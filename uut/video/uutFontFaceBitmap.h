#pragma once
#include "uutFontFace.h"

namespace uut
{
	struct UUT_API FontKerningPair
	{
		uint32_t first;
		uint32_t second;
		int16_t amount;
	};



	class UUT_API FontFaceBitmap : public FontFace
	{
	public:
		FontFaceBitmap(uint16_t size);

		virtual uint16_t GetSize() const override;
		virtual const FontGlyph* GetGlyph(uint32_t id) const override;
		virtual int16_t GetKerning(uint32_t a, uint32_t b) const override;
		virtual Texture* GetTexture(int index) const override;

		virtual void GenerateLine(List<Vertex2>& verts, const String& line) const override;

	protected:
		struct FaceInfo
		{
			uint16_t lineHeight;
			uint16_t base;
			uint16_t scaleW;
			uint16_t scaleH;
			uint16_t pages;
			uint8_t bitField;
			uint8_t alphaChnl;
			uint8_t redChnl;
			uint8_t greenChnl;
			uint8_t blueChnl;
		};

		uint16_t _size;
		List<FontGlyph> _glyphs;
		List<FontKerningPair> _kerningPairs;
		Dictionary<uint32_t, uint16_t> _glyphMap;
		FaceInfo _info;

		List<SharedPtr<Texture>> _textures;

		void UpdateGlyphsMap();
		void UpdateKerningMap();

		friend class BitmapFontLoader;
	};
}