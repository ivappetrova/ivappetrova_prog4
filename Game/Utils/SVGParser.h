#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <glm/vec2.hpp>	

class SVGParser final
{
public:
	// The only function to be called
	static bool GetVerticesFromSvgFile(const std::string& filePath, std::vector<std::vector<glm::vec2>>& vertices);

private:
	//static bool LoadGeometryFromSvgStream(unsigned char* pBlob, int blobSize, std::vector<Vector2f> &vertices);
	static void RemoveSpaces(std::string& svgString);
	static bool GetVerticesFromSvgString(std::string& svgText, std::vector<std::vector<glm::vec2>>& vertices);
	static bool GetVerticesFromPathData(const std::string& pathData, std::vector<glm::vec2>& vertices);
	static bool GetElementContent(const std::string& svgText, const std::string& elementName, std::string& elementValue, size_t& startContentPos, size_t& endContentPos);
	static bool GetAttributeValue(const std::string& svgText, const std::string& attributeName, std::string& attributeValue);


	// Skips any optional commas in the stream
	// SVG has a really funky format,
	// not sure this code works for all cases.
	// TODO: Test cases!
	static void SkipSvgComma(std::stringstream& stream, bool isRequired);
	static float ReadSvgValue(std::stringstream& stream, float defaultValue);
	static float ReadSvgValue(std::stringstream& stream, bool separatorRequired);

	// Reads a single point
	static glm::vec2 ReadSvgPoint(std::stringstream& stream);

	// Read the first point, 
	// taking into account relative and absolute positioning.
	// Stores this point, needed when path is closed
	// Advances the cursor if requested.
	static glm::vec2 FirstSvgPoint(std::stringstream& stream, glm::vec2& cursor, char cmd, bool isOpen, bool advance);

	// Read the next point, 
	// taking into account relative and absolute positioning.
	// Advances the cursor if requested.
	// Throws an exception if the figure is not open
	static glm::vec2 NextSvgPoint(std::stringstream& stream, glm::vec2& cursor, char cmd, bool isOpen, bool advance);

	// Reads next point, given only the new x coordinate
	static glm::vec2 NextSvgCoordX(std::stringstream& stream, glm::vec2& cursor, char cmd, bool isOpen);

	// Reads next point, given only the new y coordinate 
	static glm::vec2 NextSvgCoordY(std::stringstream& ssRef, glm::vec2& cursor, char cmd, bool isOpen);
};
