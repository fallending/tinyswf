/******************************************************************************
Copyright (c) 2013 jbyu. All rights reserved.
******************************************************************************/

#ifndef __TS_TAG_H__
#define __TS_TAG_H__

#include <vector>
#include <map>
#include <string>
#include "tsTypes.h"

namespace tinyswf {

typedef enum SWF_TagNames {
    TAG_END             = 0,
	TAG_SHOW_FRAME      = 1,
	TAG_DEFINE_SHAPE    = 2,
	TAG_PLACE_OBJECT    = 4,
	TAG_REMOVE_OBJECT   = 5,
	TAG_DEFINE_BITS,
	TAG_DEFINE_BUTTON,
	TAG_JPEG_TABLES,
	TAG_SET_BACKGROUND_COLOR = 9,
	TAG_DEFINE_FONT,
	TAG_DEFINE_TEXT,
	TAG_DO_ACTION           = 12,
	TAG_DEFINE_FONT_INFO,
	TAG_DEFINE_SOUND        = 14,
	TAG_START_SOUND         = 15,
	TAG_DEFINE_BUTTON_SOUND = 17,
	TAG_SOUND_STREAM_HEAD,
	TAG_SOUND_STREAM_BLOCK,
	TAG_DEFINE_BITS_LOSSLESS,
	TAG_DEFINE_BITS_JPEG2,
	TAG_DEFINE_SHAPE2       = 22,
	TAG_DEFINE_BUTTON_CXFORM,
	TAG_PROTECT,
	TAG_PLACE_OBJECT2       = 26,
	TAG_REMOVE_OBJECT2      = 28,
	TAG_DEFINE_SHAPE3       = 32,
	TAG_DEFINE_TEXT2,
	TAG_DEFINE_BUTTON2,
	TAG_DEFINE_BITS_JPEG3,
	TAG_DEFINE_BITS_LOSSLESS2,
	TAG_DEFINE_EDIT_TEXT,
	TAG_DEFINE_SPRITE       = 39,
	TAG_FRAME_LABEL         = 43,
	TAG_SOUND_STREAM_HEAD2  = 45,
	TAG_DEFINE_MORPH_SHAPE,
	TAG_DEFINE_FONT2        = 48,
	TAG_EXPORT_ASSETS       = 56,
	TAG_IMPORT_ASSETS,
	TAG_ENABLE_DEBUGGER,
	TAG_DO_INIT_ACTION,
	TAG_DEFINE_VIDEO_STREAM,
	TAG_VIDEO_FRAME,
	TAG_DEFINE_FONT_INFO2,
	TAG_ENABLE_DEBUGGER2    = 64,
	TAG_SCRIPT_LIMITS,
	TAG_SET_TAB_INDEX,
	TAG_FILE_ATTRIBUTES     = 69,
	TAG_PLACE_OBJECT3,
	TAG_IMPORT_ASSETS2,
	TAG_DEFINE_FONT_ALIGN_ZONES = 73,
	TAG_DEFINE_CSM_TEXT_SETTINGS,
	TAG_DEFINE_FONT3,
	TAG_SYMBOL_CLASS,
	TAG_METADATA,
	TAG_DEFINE_SCALING_GRID,
	TAG_DO_ABC                              = 82,
	TAG_DEFINE_SHAPE4                       = 83,
	TAG_DEFINE_MORPH_SHAPE2,
	TAG_DEFINE_SCENE_AND_FRAME_LABEL_DATA   = 86,
	TAG_DEFINE_BINARY_DATA,
	TAG_DEFINE_FONT_NAME        = 88,
	TAG_DEFINE_START_SOUND2     = 89,
	TAG_DEFINE_BITS_JPEG4       = 90,
	TAG_DEFINE_FONT4            = 91,
} SWF_TAG_NAMES;

//-----------------------------------------------------------------------------

// forward declaration
class SWF;
class ITag;
class PlaceObjectTag;
class MovieClip;
class Reader;

//-----------------------------------------------------------------------------

typedef std::map<std::string, uint32_t>     LabelList;
typedef std::vector<ITag*>					TagList;
typedef std::vector<TagList*>				FrameList;
typedef std::vector<POINT>					VertexArray;

//-----------------------------------------------------------------------------

// movieclip information
struct MovieFrames {
    FrameList _frames;
    LabelList _labels;
    RECT      _rectangle;
};

//-----------------------------------------------------------------------------

// import/export asset
struct Asset {
	enum Type {
		TYPE_EXPORT = 0,	// export symbol or store resource filename
		TYPE_SYMBOL = 1,	// import character from other swf.
		TYPE_IMPORT = 2,	// import unknown data from outside.
	};
    Type        type;
    uint32_t    handle;
	// for texture coordinate transformation
	// sw: 20.f / width of texture
	// sh: 20.f / height of texture
	// tx: x / width of texture
	// ty: y / height of texture
	MATRIX		texture;
};
const Asset kNULL_ASSET = {Asset::TYPE_EXPORT, 0};

//-----------------------------------------------------------------------------

// flash event
struct Event {
	enum Code {
		INVALID = 0,

		// These are for buttons & sprites.
		PRESS,
		RELEASE,
		RELEASE_OUTSIDE,
		ROLL_OVER,
		ROLL_OUT,
		DRAG_OVER,
		DRAG_OUT,
		KEY_PRESS,

		// These are for sprites only.
		INITIALIZE,
		LOAD,
		UNLOAD,
		ENTER_FRAME,
		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_MOVE,
		KEY_DOWN,
		KEY_UP,
		DATA,

		CONSTRUCT,
		SETFOCUS,
		KILLFOCUS,			

		// MovieClipLoader events
		ONLOAD_COMPLETE,
		ONLOAD_ERROR,
		ONLOAD_INIT,
		ONLOAD_PROGRESS,
		ONLOAD_START,

		// sound
		ON_SOUND_COMPLETE,

		EVENT_COUNT
	};
	unsigned char	m_id;
	unsigned char	m_key_code;
};

//-----------------------------------------------------------------------------

// character interface for display list
class ICharacter {
protected:
	bool   _visible;
    MATRIX *_transform;
    CXFORM *_cxform;		

public:
	enum TYPE {
		TYPE_NONE	= 0,
		TYPE_SHAPE	= 1,
		TYPE_TEXT	= 2,
		TYPE_MOVIE	= 3,
		TYPE_BUTTON = 4,
	};

	const static uint32_t kFRAME_MAXIMUM = 0xffffffff;

    virtual const RECT& getRectangle(void) const = 0;
	virtual void draw( SWF* owner ) = 0;
	virtual void update(void) = 0;
	virtual ICharacter* getTopMost(float localX, float localY, bool polygonTest) = 0;
	virtual void onEvent(Event::Code) = 0;

	virtual TYPE type() const = 0;

	void setTransform(MATRIX& mtx) { _transform = &mtx; }
	MATRIX* getTransform(void) { return _transform; }

	void setCXForm(CXFORM& mtx) { _cxform = &mtx; }
	CXFORM* getCXForm(void) { return _cxform; }

	bool visible(void) const { return _visible; }
	void setVisible(bool b) { _visible = b; }

	ICharacter()
		:_visible(true)
		,_transform(NULL)
	{}
	virtual ~ICharacter() { 
		//SWF_TRACE("delete ICharacter[%x]\n",this);
	}
};

//-----------------------------------------------------------------------------

class LineStyle {
public:
	bool read( Reader* reader, bool lineStyle2, bool support_32bit_color );

	float getWidth() const {
		return _width;
	}

    const COLOR4f& getColor(void) const { 
		return _color;
	}
		
	void print() {
		SWF_TRACE("LINESTYLE:%f, COLOR=%f,%f,%f,%f\n", _width, _color.r, _color.g, _color.b, _color.a);
	}

private:
	float		_width;
	COLOR4f		_color;
};

//-----------------------------------------------------------------------------

class FillStyle {
	friend class ShapeWithStyle;

public:
	enum Type {
		TYPE_SOLID							= 0x00,
		TYPE_LINEAR_GRADIENT				= 0x10,
		TYPE_RADIAL_GRADIENT				= 0x12,
		TYPE_FOCAL_GRADIENT					= 0x13,
		TYPE_REPEATING_BITMAP				= 0x40,
		TYPE_CLIPPED_BITMAP					= 0x41,
		TYPE_NON_SMOOTHED_REPEATING_BITMAP	= 0x42,
		TYPE_NON_SMOOTHED_CLIPPED_BITMAP	= 0x43,
		TYPE_INVALID = 0xff
	};

	FillStyle()
	:_type(TYPE_INVALID)
    ,_bitmap_id(0)
	{
		COLOR4f init = { 0,0,0,1 };
		_color = init;
	}
		
	bool read( Reader* reader, bool support_32bit_color );
		
	void print() {
        SWF_TRACE("FILLSTYLE=0x%x, bitmap=%d, color=[%.2f,%.2f,%.2f,%.2f]\n", _type, _bitmap_id, _color.r, _color.g, _color.b, _color.a);
	}
	
    uint16_t getBitmap(void) const { return _bitmap_id; }

    const MATRIX3f& getBitmapMatrix(void) const { return _bitmap_matrix; }
    const COLOR4f& getColor(void) const { return _color; }

	Type type() const { return _type; }
	
private:
	Type		_type;
	COLOR4f		_color;
	uint16_t	_bitmap_id;
	MATRIX3f	_bitmap_matrix;
#if 0
	MATRIX		_gradient_matrix;
	Gradient	_gradient;
#endif
};

//-----------------------------------------------------------------------------

// optional filter for text rendering
struct Filter {
	COLOR4f	color;
	float	offsetX;
	float	offsetY;
};

struct TextStyle {
	enum ALIGNMENT {
		ALIGN_LEFT		= 0,
		ALIGN_RIGHT		= 1,
		ALIGN_CENTER	= 2,
		ALIGN_JUSTIFY	= 3
	};

	COLOR4f		color;
	ALIGNMENT	alignment;
	bool		multiline;
	float		left_margin;
	float		right_margin;
	float		indent;
	float		leading;
	float		font_height;
	int			font_style;
	int			glyphs;
	const Filter *filter;
	std::string font_name;
};

//-----------------------------------------------------------------------------

class TagHeader {
public:
	inline uint32_t code() const { return _code; }
	inline uint32_t length() const { return _length; }
		
	bool read( Reader& reader );
	void print();

#ifdef SWF_DEBUG
    const char* name(void) const { return _tagNames[_code]; }
    static int initialize(void);
    static std::map<uint32_t, const char*> _tagNames;
#else
    const char* name(void) const { return "unknow"; }
#endif

private:
	uint32_t _code;	
	uint32_t _length;
};

//-----------------------------------------------------------------------------

class ITag {
public:
    // return false to delete this tag
    // return true to keep this tag
	virtual bool read( Reader& reader, SWF& swf, MovieFrames& data ) = 0;
	virtual void print() = 0;
	virtual void setup( MovieClip&, bool skipAction ) { SWF_UNUSED_PARAM(skipAction); }

	inline uint32_t code() const { return _header.code(); }
	inline int32_t length() const { return _header.length(); }
	const TagHeader& header() { return _header; }

    virtual ~ITag() {}

protected:
	ITag( TagHeader& header ) 
		:_header( header ) 
	{}
		
	TagHeader	_header;
};

//-----------------------------------------------------------------------------

class EndTag : public ITag {
public:
	EndTag( TagHeader& header )
        :ITag( header )
	{}
		
	virtual ~EndTag() {}
		
	virtual bool read( Reader& , SWF& , MovieFrames& data ) {
		SWF_UNUSED_PARAM(data);
		return false;
	}
	virtual void print() { SWF_TRACE("END TAG\n"); }
		
	static ITag* create( TagHeader& header ) {
		return new EndTag(header);
	}				
};

//-----------------------------------------------------------------------------

}//namespace
#endif // __TS_TAG_H__