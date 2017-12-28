#ifndef OPENCL_HELPER
#define OPENCL_HELPER

#ifdef __APPLE__
#include <OpenCL/OpenCL.h> //For Mac
#else
#include <CL/CL.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <exception>
#include <cstring>


constexpr unsigned int PLATFORM_MAX = 4;
constexpr unsigned int DEVICE_MAX = 4;

constexpr unsigned int OPENCL_READ_BUFFER = 1;
constexpr unsigned int OPENCL_WRITE_BUFFER = 2;

inline std::string ShowError(cl_int err)
{
    if(err == CL_INVALID_CONTEXT)
    {
		return std::string("CL_INVALID_CONTEXT");
    }
    else if(err == CL_INVALID_VALUE)
    {
		return std::string("CL_INVALID_VALUE");
    }
    else if(err == CL_INVALID_DEVICE)
    {
		return std::string("CL_INVALID_DEVICE");		
    }
    else if(err == CL_INVALID_BINARY)
    {
		return std::string("CL_INVALID_BINARY");
    }
    else if(err == CL_OUT_OF_HOST_MEMORY)
    {
		return std::string("CL_OUT_OF_HOST_MEMORY");
    }
	else if(err == CL_INVALID_HOST_PTR)
	{
		return std::string("CL_INVALID_HOST_PTR");
	}
	else if(err == CL_INVALID_BUFFER_SIZE)
	{
		return std::string("CL_INVALID_BUFFER_SIZE");
	}
	else if(err == CL_INVALID_MEM_OBJECT)
	{
		return std::string("CL_INVALID_MEM_OBJECT");
	}
	else if(err == CL_INVALID_COMMAND_QUEUE)
	{
		return std::string("CL_INVALID_COMMAND_QUEUE");
	}
	else if(err == CL_INVALID_EVENT_WAIT_LIST)
	{
		return std::string("CL_INVALID_WAIT_LIST");
	}
	else if(err == CL_DEVICE_NOT_FOUND)
	{
		return std::string("CL_DEVICE_NOT_FOUND");
	}
	else if(err == CL_DEVICE_NOT_AVAILABLE)
	{
		return std::string("CL_DEVICE_NOT_AVAILABLE");
	}
	else if(err == CL_INVALID_PROGRAM)
	{
		return std::string("CL_INVALID_PROGRAM");
	}
	else if(err == CL_INVALID_BUILD_OPTIONS)
	{
		return std::string("CL_INVALID_BUILD_OPTIONS");
	}
	else if(err == CL_INVALID_OPERATION)
	{
		return std::string("CL_INVALID_OPERATION");
	}
	else if(err == CL_COMPILER_NOT_AVAILABLE)
	{
		return std::string("CL_COMPILER_NOT_AVAILABLE");
	}
	else if(err == CL_BUILD_PROGRAM_FAILURE)
	{
		return std::string("CL_BUILD_PROGRAM_FAILURE");		
	}
    else if(err == CL_SUCCESS)
    {
		return std::string("CL_SUCCESS");
    }
    else
    {
		return std::string("Unknown error");
    }
}

class OpenCLException
{
private:
	std::string errorFunction;
	std::string errorDetail;
	cl_int err;
public:
	OpenCLException(cl_int err, const char* errorFunction) : err(err), errorFunction(errorFunction), errorDetail("")
	{
	}

	OpenCLException(cl_int err, const char* errorFunction, const char* errorDetail) : err(err), errorFunction(errorFunction), errorDetail(errorDetail)
	{
	}	

	std::string GetErrorFunction()
	{
		return errorFunction;
	}

	std::string GetErrorDetail()
	{
		return errorDetail;
	}

	cl_int GetErrorCode()
	{
		return err;
	}
	
	friend std::ostream& operator<<(std::ostream& os, const OpenCLException& exception);
};

std::ostream& operator<<(std::ostream& os, const OpenCLException& exception);






class OpenCLController
{
private:
	cl_context context;
	cl_command_queue queue;

public:

	OpenCLController(cl_device_type openclDeviceType);

	~OpenCLController();

	const cl_context& GetContext()
	{
		return context;
	}

	const cl_command_queue& GetQueue()
	{
		return queue;
	}	
	
private:

	cl_platform_id platforms[PLATFORM_MAX];
	cl_device_id devices[DEVICE_MAX];
	cl_uint nplatforms;
	cl_uint ndevices;

	cl_ulong constantMemSize;
	cl_bool unifiedMemoryAvailable;


public:
	inline bool UnifiedMemoryAvailable()
	{
		return (bool)unifiedMemoryAvailable;
	}

	inline cl_device_id* GetDevices()
	{
		return devices;
	}

	void Synchronize()
	{
		try
		{
			cl_int err = clFinish(queue);
			if(err != CL_SUCCESS) throw OpenCLException(err, "clFinish");
		}
		catch(OpenCLException exception)
		{
			throw exception;
		}
	}

	inline unsigned int GetConstantMemSize()
	{
		return (unsigned int)constantMemSize;
	}
};



class OpenCLProgram;


class OpenCLProgram
{
private:
	
	OpenCLController& controller;		
	cl_program program;

	



	
public:
	OpenCLProgram(OpenCLController& controller, std::string kernelFileName);


	
	~OpenCLProgram();

	const cl_program& GetProgram()
	{
		return program;
	}

	const cl_command_queue GetQueue()
	{
		return controller.GetQueue();
	}


};

class OpenCLKernel
{
private:
	std::string name;

	OpenCLProgram& program;
	cl_kernel kernel;

public:

	
	OpenCLKernel(OpenCLProgram& program, std::string name);

	
	~OpenCLKernel();
	
	template<typename T>
	void SetArgument(unsigned int argIndex, T& arg)
	{
		try
		{
			cl_int err = clSetKernelArg(kernel, (cl_uint)argIndex, sizeof(T), (void*)&arg);
			if(err != CL_SUCCESS) throw OpenCLException(err, "clSetKernelArg", name.c_str());
		}
		catch(OpenCLException exception)
		{
			throw exception;
		}
	}


	cl_event Execute(cl_uint ndim, const std::vector<size_t> &globalWorkSize, const std::vector<size_t> &localWorkSize);
	cl_event Execute(cl_uint ndim, const std::vector<size_t> &globalWorkSize);	


};

template<typename T>
class OpenCLBufferBase
{
protected:
	OpenCLController& controller;
	unsigned int size;
	unsigned int mappedSize;
	unsigned int offset;
	cl_mem buffer;

	T* mappedPointer;


	unsigned int readWriteMode;
	
public:
	OpenCLBufferBase(OpenCLController &controller, unsigned int size, cl_mem_flags rwflag)
		: controller(controller), size(size), mappedPointer(nullptr), readWriteMode(OPENCL_READ_BUFFER | OPENCL_WRITE_BUFFER)
	{
		cl_int err;
		try
		{
			if((bool)(rwflag & (CL_MEM_ALLOC_HOST_PTR | CL_MEM_USE_HOST_PTR)) && !controller.UnifiedMemoryAvailable())
			{
				throw OpenCLException(CL_INVALID_DEVICE, "clCreateBuffer");
			}
			
			buffer = clCreateBuffer(controller.GetContext(), rwflag, size*sizeof(T), nullptr, &err);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clCreateBuffer");
			}
		}
		catch(OpenCLException exception)
		{
			throw exception;
		}
	}

	~OpenCLBufferBase()
	{
		clReleaseMemObject(buffer);		
	}

	unsigned int Size()
	{
		return size;
	}

	unsigned int MappedSize()
	{
		return mappedSize;
	}
	
	const cl_mem& GetMemObject()
	{
		return buffer;
	}

	T* GetMappedPointer()
	{
		return this->mappedPointer;
	}	

	void SetReadWriteMode(unsigned int mode)
	{
		readWriteMode = mode;
	}
	
	virtual T* MapToHost(size_t offset, size_t length) = 0;

	
	virtual T* MapToHost() = 0;

	virtual T& operator[](unsigned int i) = 0;
	virtual void Unmap() = 0;
	
};


template<typename T>
class OpenCLUnifiedBuffer : public OpenCLBufferBase<T>
{
	
public:

	OpenCLUnifiedBuffer(OpenCLController &controller, unsigned int size, cl_mem_flags rwflag) //rwflag: CL_MEM_READ_WRITE,...
		: OpenCLBufferBase<T>(controller, size,
							  ((CL_MEM_READ_WRITE | CL_MEM_READ_ONLY | CL_MEM_WRITE_ONLY) & rwflag) | CL_MEM_ALLOC_HOST_PTR)
	{
	}


	
	T* MapToHost(size_t offset, size_t length)
	{
		cl_int err;

		if(offset >= this->size || (length + offset) > this->size)
		{
			throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLUnifiedBuffer::MapToHost");
		}
		
		try
		{
			if(this->mappedPointer != nullptr)
			{
				if(this->offset == offset && length == this->mappedSize)
					return this->mappedPointer;
				else
				{
					Unmap();
				}
			}
			
			this->mappedPointer = (T*)clEnqueueMapBuffer(this->controller.GetQueue(), this->buffer, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, offset*sizeof(T), length*sizeof(T), 0, nullptr, nullptr, &err);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clEnqueueMapBuffer");
			}

			this->mappedSize = length;
			this->offset = offset;
		}
		catch(OpenCLException exception)
		{
			throw exception;
		}
		return this->mappedPointer;
	}

	T* MapToHost()
	{
		return MapToHost(0, this->size);
	}
	
	void Unmap()
	{
		cl_int err;
		if(this->mappedPointer == nullptr) return;

		try
		{
			err = clEnqueueUnmapMemObject(this->controller.GetQueue(), this->buffer, this->mappedPointer, 0, nullptr, nullptr);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clEnqueueUnmapMemObject");
			}
			this->mappedPointer = nullptr;
		}
		catch(OpenCLException exception)
		{
			throw exception;
		}		
	}



	T& operator[](unsigned int i)
	{
		if(this->mappedPointer == nullptr)
		{
			MapToHost();
		}
		return *(this->mappedPointer + i);
	}

};

template<typename T>
class OpenCLDeviceBuffer : public OpenCLBufferBase<T>
{

	T* hostMemory;
public:

	OpenCLDeviceBuffer(OpenCLController &controller, unsigned int size, cl_mem_flags rwflag) //rwflag: CL_MEM_READ_WRITE,...
		: OpenCLBufferBase<T>(controller, size,
							  (CL_MEM_READ_WRITE | CL_MEM_READ_ONLY | CL_MEM_WRITE_ONLY) & rwflag)
	{
		try
		{
			hostMemory = new T[size];
		}
		catch(std::bad_alloc e)
		{
			throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLDeviceBuffer::OpenCLDeviceBuffer");
		}
	}

	~OpenCLDeviceBuffer()
	{
		delete [] hostMemory;
	}


	

	T* MapToHost(size_t offset, size_t length)
	{
		cl_int err;
		
		if(offset >= this->size || (length + offset) > this->size)
		{
			throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLUnifiedBuffer::MapToHost");
		}

		try
		{
			if(this->readWriteMode & OPENCL_READ_BUFFER)
			{
				err = clEnqueueReadBuffer(this->controller.GetQueue(), this->buffer, CL_TRUE, offset*sizeof(T), length*sizeof(T), hostMemory, 0 , nullptr, nullptr);
				if(err != CL_SUCCESS) throw OpenCLException(err, "clEnqueueReadBuffer");
			}
			this->mappedSize = length;
			this->offset = offset;
			this->mappedPointer = hostMemory;
		}
		catch(OpenCLException exception)
		{
			throw exception;
		}
		
		return this->mappedPointer;
	}	

	T* MapToHost()
	{
		return MapToHost(0, this->size);
	}
	
	void Unmap()
	{
		cl_int err;
		if(this->mappedPointer == nullptr) return;

		try
		{
			if(this->readWriteMode & OPENCL_WRITE_BUFFER)
			{
				err = clEnqueueWriteBuffer(this->controller.GetQueue(), this->buffer, CL_TRUE, this->offset*sizeof(T), this->mappedSize*sizeof(T), hostMemory, 0 , nullptr, nullptr);
				if(err != CL_SUCCESS) throw OpenCLException(err, "clEnqueueWriteBuffer");
			}			
			this->mappedPointer = nullptr;
		}
		catch(OpenCLException exception)
		{
			throw exception;
		}		
	}



	T& operator[](unsigned int i)
	{
		if(this->mappedPointer == nullptr)
		{
			MapToHost();
		}
		return *(this->mappedPointer + i);
	}

};

#endif
