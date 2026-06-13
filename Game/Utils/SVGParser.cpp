#include <algorithm>
#include <iostream>
#include <fstream>
#include "SVGParser.h"

bool SVGParser::GetVerticesFromSvgFile(const std::string& filePath, std::vector<std::vector<glm::vec2>>& vertices)
{
	std::ifstream svgStream(filePath.c_str());

	if (!svgStream)
	{
		std::cerr << "SVGParser::GetVerticesFromSvgFile(..), failed to load vertices from file " << filePath << std::endl;
		return false;
	}

	std::string svgLine;
	std::string svgString;
	while (!svgStream.eof())
	{
		getline(svgStream, svgLine);
		svgString += svgLine;
	}

	svgStream.close();
	RemoveSpaces(svgString);

	if (!GetVerticesFromSvgString(svgString, vertices))
	{
		std::cerr << "SVGParser::GetVerticesFromSvgFile(..), malformed or unsupported information in file " << filePath << std::endl;
		return false;
	}

	// viewBox is still read (required for the file to be valid) but Y is NOT flipped.
	// SDL uses top-left / y-down coordinates, same as SVG, so no flip is needed.
	std::string viewBoxValue;
	if (!GetAttributeValue(svgString, "viewBox", viewBoxValue))
	{
		std::cerr << "SVGParser::GetVerticesFromSvgFile(..), no viewbox information found in " << filePath << std::endl;
		vertices.clear();
		return false;
	}

	return true;
}

void SVGParser::RemoveSpaces(std::string& svgString)
{
	size_t foundPos{};
	while ((foundPos = svgString.find(" =")) != std::string::npos)
		svgString.replace(foundPos, 2, "=");
	while ((foundPos = svgString.find("= ")) != std::string::npos)
		svgString.replace(foundPos, 2, "=");
	while ((foundPos = svgString.find(" >")) != std::string::npos)
		svgString.replace(foundPos, 2, ">");
	while ((foundPos = svgString.find("< ")) != std::string::npos)
		svgString.replace(foundPos, 2, "<");
}

bool SVGParser::GetVerticesFromSvgString(std::string& svgString, std::vector<std::vector<glm::vec2>>& vertices)
{
	size_t startPosContent{};
	size_t endPosContent{};
	std::string pathElementContent;

	while (GetElementContent(svgString, "path", pathElementContent, startPosContent, endPosContent))
	{
		std::vector<glm::vec2> verticesVector;

		std::string pathDataValue{};
		if (!GetAttributeValue(pathElementContent, " d", pathDataValue))
		{
			std::cerr << "SVGParser::GetVerticesFromSvgString(..), path element doesn't contain a d-attribute.\n ";
			vertices.clear();
			return false;
		}
		if (!GetVerticesFromPathData(pathDataValue, verticesVector))
		{
			std::cerr << "SVGParser::GetVerticesFromSvgString(..), error while extracting vertices from the path. \n";
			vertices.clear();
			return false;
		}
		if (verticesVector.size() == 0)
		{
			std::cerr << "Empty verticesVector in GetVerticesFromSvgString(..), no vertices found in the path element" << std::endl;
			vertices.clear();
			return false;
		}
		vertices.push_back(verticesVector);
	}

	if (vertices.size() == 0)
	{
		std::cerr << "Empty vertices in GetVerticesFromSvgString(..), no path element(s) found" << std::endl;
		return false;
	}

	return true;
}

bool SVGParser::GetVerticesFromPathData(const std::string& pathData, std::vector<glm::vec2>& vertices)
{
	std::string pathCmdChars(("mMZzLlHhVvCcSsQqTtAa"));
	std::stringstream ss(pathData);

	char cmd{ 0 };
	glm::vec2 cursor{};
	glm::vec2 startPoint{};
	bool isOpen = true;

	glm::vec2 vertex{};
	char pathCommand{};
	ss >> pathCommand;
	while (!ss.eof())
	{
		if (pathCmdChars.find(pathCommand) != std::string::npos)
			cmd = pathCommand;
		else
			ss.putback(pathCommand);

		switch (cmd)
		{
		case ('Z'):
		case ('z'):
			isOpen = true;
			break;
		case ('M'):
		case ('m'):
			if (isOpen)
			{
				cursor = FirstSvgPoint(ss, cursor, cmd, isOpen, true);
				startPoint = cursor;
				vertices.push_back(cursor);
				isOpen = false;
				break;
			}
			[[fallthrough]];
		case ('L'):
		case ('l'):
			vertex = NextSvgPoint(ss, cursor, cmd, isOpen, true);
			vertices.push_back(vertex);
			break;
		case ('h'):
		case ('H'):
			vertex = NextSvgCoordX(ss, cursor, cmd, isOpen);
			vertices.push_back(vertex);
			break;
		case ('v'):
		case ('V'):
			vertex = NextSvgCoordY(ss, cursor, cmd, isOpen);
			vertices.push_back(vertex);
			break;
		case ('C'):
		case ('c'):
			std::cerr << "SVGParser::GetVerticesFromPathData,  beziers are not supported.\nHave another look at the guide, or select all nodes in inkscape and press shift + L\n";
			return false;
		default:
			std::cerr << "SVGParser::GetVerticesFromPathData, " << cmd << " is not a supported SVG command";
			return false;
		}
		ss >> pathCommand;
	}
	return true;
}

bool SVGParser::GetElementContent(const std::string& svgText, const std::string& elementName, std::string& elementContent, size_t& startContentPos, size_t& endContentPos)
{
	size_t tempStartPos{ startContentPos };
	size_t tempEndPos{ endContentPos };

	std::string startElement = "<" + elementName + ">";
	std::string endElement = "<" + elementName + "/>";
	if ((tempStartPos = svgText.find(startElement)) != std::string::npos)
	{
		tempStartPos += startElement.length();
		if ((tempEndPos = svgText.find(endElement)) != std::string::npos)
		{
			elementContent = svgText.substr(tempStartPos, tempEndPos - tempStartPos);
			startContentPos = tempStartPos;
			endContentPos = tempEndPos;
			return true;
		}
		else return false;
	}

	tempStartPos = startContentPos;
	tempEndPos = endContentPos;
	startElement = "<" + elementName;
	endElement = "/>";
	if ((tempStartPos = svgText.find(startElement, tempStartPos)) != std::string::npos)
	{
		tempStartPos += startElement.length();
		if ((tempEndPos = svgText.find(endElement)) != std::string::npos)
		{
			elementContent = svgText.substr(tempStartPos, tempEndPos - tempStartPos);
			startContentPos = tempStartPos;
			endContentPos = tempEndPos;
			return true;
		}
	}
	return false;
}

bool SVGParser::GetAttributeValue(const std::string& svgText, const std::string& attributeName, std::string& attributeValue)
{
	std::string searchAttributeName{ attributeName + "=" };
	size_t attributePos = svgText.find(searchAttributeName);
	if (attributePos == std::string::npos) return false;

	size_t openingDoubleQuotePos{ svgText.find("\"", attributePos) };
	if (openingDoubleQuotePos == std::string::npos) return false;

	size_t closingDoubleQuotePos{ svgText.find("\"", openingDoubleQuotePos + 1) };
	if (closingDoubleQuotePos == std::string::npos) return false;

	attributeValue = svgText.substr(openingDoubleQuotePos + 1, closingDoubleQuotePos - openingDoubleQuotePos - 1);
	return true;
}

void SVGParser::SkipSvgComma(std::stringstream& svgStream, bool isRequired)
{
	while (true)
	{
		char c = char(svgStream.get());
		if (svgStream.eof()) { if (isRequired) std::cerr << "SVGParser::SkipSvgComma, expected comma or whitespace\n"; break; }
		if (c == (',')) return;
		if (!isspace(c)) { svgStream.unget(); return; }
	}
}

float SVGParser::ReadSvgValue(std::stringstream& svgStream, float defaultValue)
{
	float s{};
	svgStream >> s;
	if (svgStream.eof()) s = defaultValue;
	else SkipSvgComma(svgStream, false);
	return s;
}

float SVGParser::ReadSvgValue(std::stringstream& svgStream, bool separatorRequired)
{
	float s{};
	svgStream >> s;
	SkipSvgComma(svgStream, separatorRequired);
	return s;
}

glm::vec2 SVGParser::ReadSvgPoint(std::stringstream& svgStream)
{
	glm::vec2 p{};
	p.x = ReadSvgValue(svgStream, true);
	p.y = ReadSvgValue(svgStream, false);
	return p;
}

glm::vec2 SVGParser::FirstSvgPoint(std::stringstream& svgStream, glm::vec2& cursor, char cmd, bool isOpen, bool advance)
{
	if (!isOpen) std::cerr << "SVGParser::FirstSvgPoint, expected 'Z' or 'z' command";
	glm::vec2 p = ReadSvgPoint(svgStream);
	if (islower(cmd)) { p.x = cursor.x + p.x; p.y = cursor.y + p.y; }
	if (advance) cursor = p;
	return p;
}

glm::vec2 SVGParser::NextSvgPoint(std::stringstream& svgStream, glm::vec2& cursor, char cmd, bool isOpen, bool advance)
{
	if (isOpen) std::cerr << "SVGParser::NextSvgPoint, expected 'M' or 'm' command\n";
	glm::vec2 p = ReadSvgPoint(svgStream);
	if (islower(cmd)) { p.x = cursor.x + p.x; p.y = cursor.y + p.y; }
	if (advance) cursor = p;
	return p;
}

glm::vec2 SVGParser::NextSvgCoordX(std::stringstream& svgStream, glm::vec2& cursor, char cmd, bool isOpen)
{
	if (isOpen) std::cerr << "SVGParser::NextSvgCoordX, expected 'M' or 'm' command\n";
	float c; svgStream >> c;
	if (islower(cmd)) cursor.x += c;
	else cursor.x = c;
	return cursor;
}

glm::vec2 SVGParser::NextSvgCoordY(std::stringstream& svgStream, glm::vec2& cursor, char cmd, bool isOpen)
{
	if (isOpen) std::cerr << "SVGParser::NextSvgCoordY, expected 'M' or 'm' command\n";
	float c{}; svgStream >> c;
	if (islower(cmd)) cursor.y += c;
	else cursor.y = c;
	return cursor;
}