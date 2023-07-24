#pragma once
#include <memory>
#ifdef _WIN32
#else
#include <string.h>
#endif 

class BufferRaw
{
public:
	typedef std::shared_ptr<BufferRaw> Ptr;
	BufferRaw(uint32_t capacity = 1500)
	{
		if (capacity)
			setCapacity(capacity);
	}
	BufferRaw(const char* data, int size)
	{
		assign(data, size);
	}

	~BufferRaw()
	{
		if (_data)
			delete[] _data;
	}

	void assign(const char* data, uint32_t size)
	{
		setCapacity(size + 1);
		memcpy(_data, data, size);
		_data[size] = '\0';
		_size = size;
	}

	void append(const char* data, uint32_t size)
	{
		if (_size + size + 1 > _capacity)
		{
			char* newData = new char[_size + size + 1];
			_capacity = _size + size + 1;
			memcpy(newData, _data, _size);
			delete[] _data;
			_data = newData;
		}
		memcpy(_data + _size, data, size);
		_size += size;
		_data[_size] = '\0';
	}

	void clear()
	{
		_size = 0;
		_data[0] = '\0';
	}

	char* data() const
	{
		return _data;
	}

	uint32_t size() const
	{
		return _size;
	}

	void setSize(uint32_t sz)
	{
		_size = sz;
	}

	void setCapacity(uint32_t capacity)
	{
		if (_data)
		{
			do
			{
				if (capacity > _capacity)//当前容量过小重新开辟内存
					break;

				if (_capacity < 2 * 1024) //上次内存超过2k，下次分配一个较小的内存
					return;
			} while (false);
			delete[] _data;
		}
		_data = new char[capacity];
		_capacity = capacity;
	}

private:
	char* _data = nullptr;
	uint32_t _capacity = 0;
	uint32_t _size = 0;
};

