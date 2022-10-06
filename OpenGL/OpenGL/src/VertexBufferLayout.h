#pragma once

#include <vector>
#include "Renderer.h"

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT:			return 4; break;
			case GL_UNSIGNED_INT:	return 4; break;
			case GL_UNSIGNED_BYTE:	return 1; break;
		
		default:break;
		}
		ASSERT(false);
		return 0;
	}
};

// ͼ����
class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		:m_Stride(0) {}

	// ģ��ƫ�ػ�?
	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(true);
	}

	// ģ���ػ�
	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	// �������� ������дcpp
	inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }	// ��Ҫ����const& ��
	inline unsigned int GetStride() const& { return m_Stride; }
};

