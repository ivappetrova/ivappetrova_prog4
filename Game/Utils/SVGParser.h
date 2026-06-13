// Credits to PROGRAMMING 2- Engine

#ifndef SVG_PARSER_H
#define SVG_PARSER_H

#include <string>
#include <vector>
#include <sstream>
#include <glm/vec2.hpp>	

class SVGParser final
{
public:
	static bool GetVerticesFromSvgFile(const std::string& filePath, std::vector<std::vector<glm::vec2>>& vertices);

private:
	static void RemoveSpaces(std::string& svgString);
	static bool GetVerticesFromSvgString(std::string& svgText, std::vector<std::vector<glm::vec2>>& vertices);
	static bool GetVerticesFromPathData(const std::string& pathData, std::vector<glm::vec2>& vertices);
	static bool GetElementContent(const std::string& svgText, const std::string& elementName, std::string& elementValue, size_t& startContentPos, size_t& endContentPos);
	static bool GetAttributeValue(const std::string& svgText, const std::string& attributeName, std::string& attributeValue);

	static void SkipSvgComma(std::stringstream& stream, bool isRequired);
	static float ReadSvgValue(std::stringstream& stream, float defaultValue);
	static float ReadSvgValue(std::stringstream& stream, bool separatorRequired);

	static glm::vec2 ReadSvgPoint(std::stringstream& stream);

	static glm::vec2 FirstSvgPoint(std::stringstream& stream, glm::vec2& cursor, char cmd, bool isOpen, bool advance);

	static glm::vec2 NextSvgPoint(std::stringstream& stream, glm::vec2& cursor, char cmd, bool isOpen, bool advance);

	static glm::vec2 NextSvgCoordX(std::stringstream& stream, glm::vec2& cursor, char cmd, bool isOpen);

	static glm::vec2 NextSvgCoordY(std::stringstream& ssRef, glm::vec2& cursor, char cmd, bool isOpen);
};

#endif 