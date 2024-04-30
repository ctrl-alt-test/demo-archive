#ifndef RESOURCE_ID_HH
#define RESOURCE_ID_HH

namespace Gfx
{
	struct VertexBufferID
	{
		int index;
		static const VertexBufferID InvalidID;

		inline bool operator == (const VertexBufferID& rhs) const
		{
			return index == rhs.index;
		}

		inline bool operator != (const VertexBufferID& rhs) const
		{
			return !(*this == rhs);
		}
	};

	struct TextureID
	{
		int index;
		static const TextureID InvalidID;

		inline bool operator == (const TextureID& rhs) const
		{
			return index == rhs.index;
		}

		inline bool operator != (const TextureID& rhs) const
		{
			return !(*this == rhs);
		}
	};

	struct ShaderID
	{
		int index;
		static const ShaderID InvalidID;

		inline bool operator == (const ShaderID& rhs) const
		{
			return index == rhs.index;
		}

		inline bool operator != (const ShaderID& rhs) const
		{
			return !(*this == rhs);
		}
	};

	struct FrameBufferID
	{
		int index;
		static const FrameBufferID InvalidID;

		inline bool operator == (const FrameBufferID& rhs) const
		{
			return index == rhs.index;
		}

		inline bool operator != (const FrameBufferID& rhs) const
		{
			return !(*this == rhs);
		}
	};
}

#endif // RESOURCE_ID_HH
