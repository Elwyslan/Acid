#include "Xml.hpp"

#include "Files/Files.hpp"

namespace acid
{
Xml::Xml(const std::string &rootName) :
	Metadata{rootName}
{
}

Xml::Xml(const std::string &rootName, Metadata *metadata) :
	Metadata{rootName}
{
	AddChildren(metadata, this);
}

void Xml::Load(std::istream *inStream)
{
	ClearChildren();
	ClearAttributes();

	auto topNode{std::make_unique<Node>(nullptr, "", "")};
	Node *currentSection{};
	std::stringstream summation;
	bool end{};

	std::size_t lineNum{};
	std::string linebuf;

	while (inStream->peek() != -1)
	{
		Files::SafeGetLine(*inStream, linebuf);
		lineNum++;

		for (auto it{linebuf.begin()}; it != linebuf.end(); ++it)
		{
			if (*it == '<')
			{
				if (*(it + 1) == '?') // Prolog.
				{
					currentSection = topNode.get();
					continue;
				}

				if (*(it + 1) == '/') // End tag.
				{
					currentSection->m_content += summation.str();
					end = true;
				}
				else // Start tag.
				{
					auto section{new Node{currentSection, "", ""}};
					currentSection->m_children.emplace_back(section);
					currentSection = section;
				}

				summation.str({});
			}
			else if (*it == '>')
			{
				if (!end)
				{
					currentSection->m_attributes += summation.str();
				}

				summation.str({});

				if (end || *(it - 1) == '/') // End tag.
				{
					end = false;

					if (currentSection->m_parent != nullptr)
					{
						currentSection = currentSection->m_parent;
					}
				}
			}
			else if (*it == '\n')
			{
			}
			else
			{
				summation << *it;
			}
		}
	}

	if (!topNode->m_children.empty())
	{
		Convert(topNode->m_children[0].get(), this, 1);
	}
}

void Xml::Write(std::ostream *outStream, const Format &format) const
{
	*outStream << R"(<?xml version="1.0" encoding="utf-8"?>)";
	
	if (format != Format::Minified)
	{
		*outStream << '\n';
	}

	AppendData(this, outStream, 0, format);
}

void Xml::Load(const std::string &string)
{
	std::stringstream stream{string};
	Load(&stream);
}

std::string Xml::Write(const Format &format) const
{
	std::stringstream stream;
	Write(&stream, format);
	return stream.str();
}

void Xml::AddChildren(const Metadata *source, Metadata *destination)
{
	for (const auto &child : source->GetChildren())
	{
		auto created{destination->AddChild(std::make_unique<Metadata>(child->GetName(), child->GetValue()))};
		AddChildren(child.get(), created);
	}

	for (const auto &attribute : source->GetAttributes())
	{
		destination->SetAttribute(attribute.first, attribute.second);
	}
}

void Xml::Convert(const Node *source, Metadata *parent, const uint32_t &depth)
{
	auto firstSpace{String::FindCharPos(source->m_attributes, ' ')};
	auto name{String::Trim(source->m_attributes.substr(0, firstSpace))};
	auto attributes{source->m_attributes.substr(firstSpace + 1, source->m_attributes.size() - (firstSpace + 1))};
	attributes = String::Trim(attributes);

	if (attributes[attributes.size() - 1] == '/' || attributes[attributes.size() - 1] == '?')
	{
		attributes.pop_back();
	}

	attributes = String::Trim(attributes);

	std::map<std::string, std::string> parseAttributes;

	if (!attributes.empty())
	{
		std::string currentKey;
		std::string summation;

		for (const auto &c : attributes)
		{
			switch (c)
			{
			case '"':
			{
				if (currentKey.empty())
				{
					currentKey = summation;
					summation.clear();
					continue;
				}

				parseAttributes.emplace(String::Trim(currentKey), String::Trim(summation));
				currentKey.clear();
				summation.clear();
				break;
			}
			case '=':
			{
				if (!currentKey.empty())
				{
					summation += c;
				}

				break;
			}
			default:
			{
				summation += c;
				break;
			}
			}
		}
	}

	auto thisValue{parent};

	if (depth != 0)
	{
		if (depth != 1)
		{
			thisValue = parent->AddChild(std::make_unique<Metadata>(name, source->m_content, parseAttributes));
		}
		else
		{
			thisValue->SetName(name);
			thisValue->SetValue(source->m_content);
			thisValue->SetAttributes(parseAttributes);
		}

		for (const auto &child : source->m_children)
		{
			Convert(child.get(), thisValue, depth + 1);
		}
	}
	else
	{
		parent->SetName(name);
		parent->SetValue(source->m_content);
		parent->SetAttributes(parseAttributes);
	}
}

void Xml::AppendData(const Metadata *source, std::ostream *outStream, const int32_t &indentation, const Format &format)
{
	std::stringstream indents;

	if (format != Format::Minified)
	{
		for (int32_t i{}; i < indentation; i++)
		{
			indents << "  ";
		}
	}

	auto name{String::ReplaceAll(source->GetName(), " ", "_")};

	std::stringstream nameAttributes;
	nameAttributes << name;

	for (const auto &[attributeName, value] : source->GetAttributes())
	{
		nameAttributes << " " << attributeName << "=\"" << value << "\"";
	}

	auto nameAndAttribs{String::Trim(nameAttributes.str())};

	*outStream << indents.str();

	if (source->GetName()[0] == '?')
	{
		*outStream << "<" << nameAndAttribs << "?>";

		if (format != Format::Minified)
		{
			*outStream << '\n';
		}

		for (const auto &child : source->GetChildren())
		{
			AppendData(child.get(), outStream, indentation, format);
		}

		return;
	}

	if (source->GetChildren().empty() && source->GetValue().empty())
	{
		*outStream << "<" << nameAndAttribs << "/>";

		if (format != Format::Minified)
		{
			*outStream << '\n';
		}

		return;
	}

	*outStream << "<" << nameAndAttribs << ">" << String::FixReturnTokens(source->GetValue());

	if (!source->GetChildren().empty())
	{
		if (format != Format::Minified)
		{
			*outStream << '\n';
		}

		for (const auto &child : source->GetChildren())
		{
			AppendData(child.get(), outStream, indentation + 1, format);
		}

		*outStream << indents.str();
	}

	*outStream << "</" << name << '>';

	if (format != Format::Minified)
	{
		*outStream << '\n';
	}
}
}
