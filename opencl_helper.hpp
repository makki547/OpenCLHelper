#ifndef OPENCL_HELPER
#define OPENCL_HELPER

#ifdef __APPLE__
#include <OpenCL/OpenCL.h> //For Mac
#else
#pragma warning(disable : 4996)
#include <CL/cl.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <exception>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <memory>
#include <utility>
#include <regex>


namespace opencl_helper
{
	class OpenCLException;
	class OpenCLDevice;
	class OpenCLPlatforms;
}

std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLException& exception);

std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLDevice& device);
std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLPlatforms& platforms);


namespace opencl_helper
{

	constexpr unsigned int PLATFORM_MAX = 10;
	constexpr unsigned int DEVICE_MAX = 10;

	constexpr unsigned int OPENCL_READ_BUFFER = 1;
	constexpr unsigned int OPENCL_WRITE_BUFFER = 2;

	
	//Deleter of OpenCL objects for unique_ or shared_ptr
	class OpenCLReleaseContext
	{
	public:
		void operator()(cl_context context)
		{
			clReleaseContext(context);
		}
	};

	class OpenCLReleaseQueue
	{
	public:
		void operator()(cl_command_queue queue)
		{
			clReleaseCommandQueue(queue);
		}
	};	

	class OpenCLReleaseProgram
	{
	public:
		void operator()(cl_program program)
		{
			clReleaseProgram(program);
		}
	};

	class OpenCLReleaseKernel
	{
	public:
		void operator()(cl_kernel kernel)
		{
			clReleaseKernel(kernel);
		}
	};


	class OpenCLReleaseMemObject
	{
	public:
		void operator()(cl_mem mem)
		{
			clReleaseMemObject(mem);
		}
	};

	class OpenCLReleaseEvent
	{
	public:
		void operator()(cl_event event)
		{
			clReleaseEvent(event);
		}
	};
	
	
	typedef std::shared_ptr<_cl_context> ContextSharedPtr;
	typedef std::shared_ptr<_cl_command_queue> CommandQueueSharedPtr;
	typedef std::weak_ptr<_cl_context> ContextWeakPtr;
	typedef std::weak_ptr<_cl_command_queue> CommandQueueWeakPtr;
	typedef CommandQueueWeakPtr OpenCLCommandQueue;
	typedef std::shared_ptr<_cl_program> ProgramSharedPtr;
	typedef std::weak_ptr<_cl_program> ProgramWeakPtr;
	typedef std::unique_ptr<_cl_mem, OpenCLReleaseMemObject> MemObjectUniquePtr;	
	typedef std::unique_ptr<_cl_kernel, OpenCLReleaseKernel> KernelUniquePtr;
	typedef std::shared_ptr<_cl_event> EventSharedPtr;
	typedef std::weak_ptr<_cl_event> EventWeakPtr;


	typedef std::vector<cl_device_id> OpenCLDeviceIDList;	
	
	class OpenCLException
	{
	private:
		std::string errorFunction;
		std::string errorDetail;
		std::string errorName;
		cl_int err;

		std::string GetErrorName(cl_int err);
		
	public:
		OpenCLException(cl_int err, const char* errorFunction) : err(err), errorFunction(errorFunction), errorDetail("")
		{
			errorName = GetErrorName(err);
		}

		OpenCLException(cl_int err, const char* errorFunction, const char* errorDetail) : err(err), errorFunction(errorFunction), errorDetail(errorDetail)
		{
			errorName = GetErrorName(err);			
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

		std::string ShowErrorName()
		{
			return errorName;
		}
	
		friend std::ostream& ::operator<<(std::ostream& os, const opencl_helper::OpenCLException& exception);
	};


	template<class Function>
	class OpenCLEventCallback
	{
	private:
		Function function;
	public:
		OpenCLEventCallback(Function function)
			:function(function)
		{
		}

		OpenCLEventCallback(Function &&function)
		{
			OpenCLEventCallback::function = std::move(function);
		}		


		static void Callback(cl_event event, cl_int eventCmdExecStatus, void *self)
		{
			
			static_cast< OpenCLEventCallback<Function>* >(self)->function(eventCmdExecStatus);
		}
		
		
	};	

	
	class OpenCLEvent
	{
	private:
		EventSharedPtr event;
		cl_int status;
		cl_command_type cmdType;

	public:
		OpenCLEvent()
		{
		}
		
		OpenCLEvent(cl_event event)
		{
			OpenCLEvent::event = EventSharedPtr(event, OpenCLReleaseEvent());
		}

		EventWeakPtr GetEvent()
		{
			return std::move(EventWeakPtr(event));
		}

		cl_int GetStatus()
		{

			if(event)
			{
				cl_int err = clGetEventInfo(event.get(), CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), (void*)&status, nullptr);
				if(err != CL_SUCCESS)
				{
					throw OpenCLException(err, "clGetEventInfo");
				}
			}
			return status;
		}

		template <template <class...> class CallbackClass, class Function>
		void SetCallback(CallbackClass<Function> &callback, cl_int cmdExecCallBackType)
		{
			cl_int err = clSetEventCallback(event.get(), cmdExecCallBackType, callback.Callback, &callback);

			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clSetEventCallback");
			}
			
		}

		void FlushUserEvent()
		{
			cl_int err = clSetUserEventStatus(event.get(), CL_COMPLETE);

			if(err != CL_SUCCESS)
			{
				if(err == CL_INVALID_EVENT)
				{
					throw OpenCLException(err, "clSetUserEvent", "Non user-event might be operated.");
				}
				else
				{
					throw OpenCLException(err, "clSetUserEvent");
				}
			}
		}

		cl_command_type GetEventCommandType()
		{
			if(event)
			{
				cl_int err = clGetEventInfo(event.get(), CL_EVENT_COMMAND_TYPE, sizeof(cl_command_type), (void*)&cmdType, nullptr);
				if(err != CL_SUCCESS)
				{
					throw OpenCLException(err, "clGetEventInfo");
				}
			}
			return cmdType;
		}
	};

	class OpenCLEventList : public std::vector<cl_event>
	{
	public:
		void push_back(OpenCLEvent &event)
		{
			EventWeakPtr ptr = event.GetEvent();
			if(ptr.expired())
			{
				throw OpenCLException(CL_INVALID_EVENT, "OpenCLEventList::push_back");
			}

			std::vector<cl_event>::push_back(ptr.lock().get());
		}

		cl_event* GetList()
		{
			if(empty())
			{
				throw OpenCLException(CL_INVALID_EVENT, "OpenCLEventList::GetList");
			}

			return &at(0);
		}

		void Wait()
		{
			cl_int err = clWaitForEvents(size(), GetList());
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clWaitForEvent");
			}
		}
	};

	class OpenCLEvents : public std::unordered_map<std::string, OpenCLEvent>
	{
	public:
		

		void RemoveComplete()
		{
			std::vector<std::string> completeKeys;
			completeKeys.reserve(size());
			for(auto itr = begin();itr != itr;++itr)
			{
				if(itr->second.GetStatus() == CL_COMPLETE)
				{
					completeKeys.push_back(itr->first);
				}
			}

			for(auto itr = completeKeys.begin();itr != completeKeys.end();++itr)
			{
				erase(*itr);
			}
		}

		OpenCLEventList GetByKeys(const std::vector<std::string> &keys)
		{
			OpenCLEventList eventList;
			eventList.reserve(keys.size());
			for(auto itr = keys.begin();itr != keys.end();++itr)
			{
				auto res = find(*itr);
				if(res != end())
				{
					eventList.push_back(res->second);
				}
			}

			return std::move(eventList);
		}



		OpenCLEventList GetByRegex(const std::string pattern)
		{
			OpenCLEventList eventList;
			eventList.reserve(size());
			try
			{
				std::regex regex(pattern.begin(), pattern.end());
				for(auto itr = begin();itr != itr;++itr)
				{
					if(std::regex_match(itr->first.begin(), itr->first.end(), regex))
					{
						eventList.push_back(itr->second);
					}
				}				
			}
			catch(std::regex_error rerror)
			{
				throw OpenCLException(CL_INVALID_EVENT, "OpenCLEvents::GetByRegex", "Invalid regex");
			}
			return eventList;
		}
	};


	class OpenCLDevice
	{
	private:
		cl_bool available;

		cl_bool hostUnifiedMemory;
		std::string vendor;
		std::string name;
		std::string version;


		cl_device_id deviceID;
	public:

		OpenCLDevice(const cl_device_id deviceID);
		OpenCLDevice(const OpenCLDevice &) = default;
		OpenCLDevice(OpenCLDevice &&) = default;


		/*
		  cl_device_info and its return type are described in following URL.
		  https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/clGetDeviceInfo.html

		  If you want to obtain informations those return type is char[],
		  then you should use overloaded function that returns std::string.

		  Example:
		  cl_ulong globalMemCacheSize = openClDevice.GetInformation<cl_ulong>(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
		  std::string vendor = openCLDevice.GetInformation(CL_DEVICE_VENDOR);
		*/
		template<typename T>
		T GetInformation(const cl_device_info paramName)
		{
			cl_int err;
			T res;

			err = clGetDeviceInfo(deviceID, paramName, sizeof(T), &res, nullptr);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clGetDeviceInfo");
			}

			return res;
		}


		std::string GetInformation(const cl_device_info paramName)
		{
			
			cl_int err;
			char strres[1024] = {0};

			err = clGetDeviceInfo(deviceID, paramName, sizeof(strres), &strres, nullptr);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clGetDeviceInfo");
			}
			return std::move(std::string(strres));
		}

		cl_device_id GetDeviceID()
		{
			return deviceID;
		}

		explicit operator bool() const noexcept
		{
			return (bool)available;
		}

		bool UnifiedMemoryAvailable()
		{
			return (bool)hostUnifiedMemory;
		}

		std::string GetName()
		{
			return name;
		}

		std::string GetVendor()
		{
			return vendor;
		}
		std::string GetVersion()
		{
			return version;
		}
		

		friend std::ostream& ::operator<<(std::ostream& os, const opencl_helper::OpenCLDevice& device);
	};


	class OpenCLDevices : public std::vector<OpenCLDevice>
	{
	public:
		OpenCLDeviceIDList GetDeviceIDList();
		
	};

	struct PlatformInformation
	{
		cl_platform_id platformID;
		std::string vendor;
		std::string version;

		OpenCLDevices devices;
	};

	class OpenCLPlatformInformations : public std::vector<PlatformInformation>
	{
	public:
		std::vector<std::string> GetVendors();
		std::vector<std::string> GetVersions();
	};

	
	class OpenCLPlatforms
	{
		
	private:
		


		OpenCLPlatformInformations platformInformations;

	public:
		OpenCLPlatforms(cl_device_type openclDeviceType); //cl_device_type = CL_DEVICE_TYPE_ALL, CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU,...

		unsigned int CountPlatforms()
		{
			return platformInformations.size();
		}

		unsigned int CountDevices(unsigned int platformIndex);



		//Get device list that is used to create OpenCLContext. Devices in the seleted platform should be selected.
		OpenCLDevices GetDeviceList(unsigned int platformIndex, std::vector<unsigned int> deviceIndices);

		//Get device list that is used to create OpenCLContext. All devices in the seleted platform are used.
		OpenCLDevices GetDeviceList(unsigned int platformIndex);

		std::vector<std::string> GetVendors()
		{
			return std::move(platformInformations.GetVendors());
		}

		std::vector<std::string> GetVersions()
		{
			return std::move(platformInformations.GetVersions());
		}

		friend std::ostream& ::operator<<(std::ostream& os, const opencl_helper::OpenCLPlatforms& platforms);		

	};




	
	class OpenCLDeviceQueue
	{
	private:
		CommandQueueSharedPtr queue;
		OpenCLDevice device;

		
	public:

		OpenCLDeviceQueue(const OpenCLDevice &device, ContextWeakPtr context, bool EnableOutOfOrderMode = false);

		explicit operator bool() const noexcept			
		{
			return queue ? true : false;
		}

		CommandQueueWeakPtr GetQueue()
		{
			return std::move(CommandQueueWeakPtr(queue));
		}
/*
		void EnableInOrderMode()
		{
			cl_int err = clSetCommandQueueProperty(queue.get(), CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, CL_TRUE, nullptr);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clSetCommandQueueProperty");
			}
		}

		void DisableInOrderMode()
		{
			cl_int err = clSetCommandQueueProperty(queue.get(), CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, CL_FALSE, nullptr);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clSetCommandQueueProperty");				
			}
		}		
*/	
	};


	typedef std::vector<OpenCLDeviceQueue> DeviceQueueList;
	
	class OpenCLContext
	{
	private:
		OpenCLDevices devices;
		ContextSharedPtr context;
		DeviceQueueList queues;

		bool unifiedMemoryAvailable;

	public:
		//It is recommended that devices are provided from OpenCLPlatforms::GetDeviceList
		OpenCLContext(const OpenCLDevices& devices, bool enableOutOfOrderMode = false);

		OpenCLContext(OpenCLContext &&) = delete;
		OpenCLContext& operator=(OpenCLContext &&) = delete;

		//This deviceIndex is not always equal to that in OPenCLPlatforms.
		//These indices are corresponded to devices those were used to make the context
		OpenCLCommandQueue GetQueue(unsigned int deviceIndex = 0);

		void WaitDevice(unsigned int deviceIndex);

		void Synchronize();


		OpenCLDeviceIDList GetDeviceIDList()
		{
			return std::move(devices.GetDeviceIDList());
		}

		bool UnifiedMemoryAvailable()
		{
			return unifiedMemoryAvailable;
		}

		ContextWeakPtr GetContext()
		{
			return std::move(ContextWeakPtr(context));
		}

		//Device information selected by deviceIndex can be obtained.
		//The details are written in OpenCLDevice::GetInformation.
		template<typename T>
		T GetDeviceInformation(unsigned int deviceIndex, const cl_device_info paramName)
		{
			T t;
			try
			{
				t = devices.at(deviceIndex).GetInformation<T>(paramName);
			}
			catch(std::out_of_range e)
			{
				throw OpenCLException(CL_INVALID_DEVICE, "OpenCLContext::GetDeviceInformation");
			}
			catch(OpenCLException e)
			{
				throw e;
			}
			return t;
		}

		std::string GetDeviceInformation(unsigned int deviceIndex, const cl_device_info paramName)
		{
			std::string t;
			try
			{
				t = devices.at(deviceIndex).GetInformation(paramName);
			}
			catch(std::out_of_range e)
			{
				throw OpenCLException(CL_INVALID_DEVICE, "OpenCLContext::GetDeviceInformation");
			}
			catch(OpenCLException e)
			{
				throw e;
			}
			return t;
		}


		OpenCLEvent CreateUserEvent()
		{
			cl_int err;

			cl_event event = clCreateUserEvent(context.get(), &err);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clCreateUserEvent");
			}
			return std::move(OpenCLEvent(event));
		}
/*
		void EnableInOrderMode()
		{
			for(auto itr = queues.begin();itr != queues.end();++itr)
			{
				itr->EnableInOrderMode();
			}
		}

		void DisableInOrderMode()
		{
			for(auto itr = queues.begin();itr != queues.end();++itr)
			{
				itr->DisableInOrderMode();
			}
		}		
*/
	};





	class OpenCLKernel;

	class OpenCLKernels;



	
	class OpenCLProgram
	{
	private:
	


		ProgramSharedPtr program;
		OpenCLContext* context;
		//ContextWeakPtr context;
		
	



	
	public:
		//OpenCL source code that is placed in kernelFileName can be compiled.
		OpenCLProgram(OpenCLContext &context, std::string kernelFileName);		

	

		ProgramWeakPtr GetProgram()
		{
			return std::move(ProgramWeakPtr(program));
		}
		

		OpenCLKernels GetKernels();

		OpenCLContext* GetContext()
		{
			return context;
		}


	};


  
		
	class OpenCLKernel
	{
	private:

		struct Argument
		{
			void* address;
			cl_int size;
		public:
			Argument()
			{
				address = nullptr;
				size = 0;
			}
		};
		
		std::string name;

		OpenCLContext *context;
		//OpenCLProgram& program;
		//OpenCLProgram* program; //Change from reference to pointer to define copy constructor (because program could not be overwritten)
		ProgramWeakPtr program;
		KernelUniquePtr kernel;
//		cl_kernel kernel;

		std::vector<Argument> arguments; //The address and its size of arguments are stored to retain arguments in copied objects.

	public:

		//A kernel-function that is named 'name' is loaded from program
		OpenCLKernel(OpenCLProgram& program, std::string name);
		OpenCLKernel(OpenCLProgram& program, cl_kernel kernel);		
		OpenCLKernel(const OpenCLKernel &openclKernel);

		OpenCLKernel& operator=(const OpenCLKernel &src);
		
	
		template<typename T>
		void SetArgument(unsigned int argIndex, T& arg)
		{

			cl_int err = clSetKernelArg(kernel.get(), (cl_uint)argIndex, sizeof(T), (void*)&arg);
			if(err != CL_SUCCESS) throw OpenCLException(err, "clSetKernelArg", name.c_str());
			arguments[argIndex].address = (void*)&arg;
			arguments[argIndex].size = sizeof(T);
		}

		void SetArgument(unsigned int argIndex, cl_mem arg)
		{

			cl_int err = clSetKernelArg(kernel.get(), (cl_uint)argIndex, sizeof(cl_mem), (void*)&arg);
			if(err != CL_SUCCESS) throw OpenCLException(err, "clSetKernelArg", name.c_str());
			arguments[argIndex].address = (void*)&arg;
			arguments[argIndex].size = sizeof(cl_mem);
		}		


		std::string GetName()
		{
			return name;
		}


		OpenCLEvent Execute(CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize, const std::vector<size_t> &localWorkSize);
		OpenCLEvent Execute(CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize);


		OpenCLEvent Execute(CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize, const std::vector<size_t> &localWorkSize, OpenCLEventList &eventWaitList);
		OpenCLEvent Execute(CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize, OpenCLEventList &eventWaitList);			


	};

	class OpenCLKernels : public std::unordered_map<std::string, OpenCLKernel>
	{

	public:

		
		OpenCLKernel& operator[](std::string kernelName)
		{
			OpenCLKernel *p;
			try
			{
				p = &(at(kernelName));
			}
			catch(std::out_of_range e)				
			{
				throw OpenCLException(CL_INVALID_KERNEL, "OpenCLKernels::operator[]");
			}
			return *p;
		}
	};	

	
	template<typename T>
	class OpenCLBufferBase
	{
	protected:
		OpenCLContext& context;
		unsigned int size;
		unsigned int mappedSize;
		unsigned int offset;

		MemObjectUniquePtr buffer;
		cl_mem_flags rwflag;
		
		T* mappedPointer;


		unsigned int readWriteMode;
		//This is used in OpenCLDeviceBuffer.
		//In the case a bit frag of OPENCL_READ_BUFFER is true, then data are copied into host memory, when MapToHost is called.
		//In the case a bit frag of OPENCL_WRITE_BUFFER is true, then data are copied into device memory, when Unmap is called.
		//Appropriate settings will reduce data tranfer cost.
		CommandQueueWeakPtr queueForUnmap;
		CommandQueueWeakPtr defaultQueue;

	
	public:
		OpenCLBufferBase(OpenCLContext &context, unsigned int size, cl_mem_flags rwflag)
			: context(context), size(size), mappedPointer(nullptr), readWriteMode(OPENCL_READ_BUFFER | OPENCL_WRITE_BUFFER), rwflag(rwflag)
		{
			cl_int err;

			if((bool)(rwflag & (CL_MEM_ALLOC_HOST_PTR | CL_MEM_USE_HOST_PTR)) && !context.UnifiedMemoryAvailable())
			{
				throw OpenCLException(CL_INVALID_DEVICE, "clCreateBuffer");
			}

			ContextWeakPtr wcon = context.GetContext();
			if(wcon.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLBufferBase::OpenCLBufferBase");
			}

			cl_mem buf = clCreateBuffer(wcon.lock().get(), rwflag, size*sizeof(T), nullptr, &err);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clCreateBuffer");
			}

			buffer = MemObjectUniquePtr(buf);
			defaultQueue = context.GetQueue(0);

		}

		OpenCLBufferBase(const OpenCLBufferBase &src)
			: context(src.context), size(src.size), mappedPointer(src.mappedPointer), mappedSize(src.mappedSize), offset(src.offset),
			  readWriteMode(src.readWriteMode), rwflag(src.rwflag), defaultQueue(src.defaultQueue)
		{
			cl_int err;

			if((bool)(rwflag & (CL_MEM_ALLOC_HOST_PTR | CL_MEM_USE_HOST_PTR)) && !context.UnifiedMemoryAvailable())
			{
				throw OpenCLException(CL_INVALID_DEVICE, "clCreateBuffer");
			}

			ContextWeakPtr wcon = context.GetContext();
			if(wcon.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLBufferBase::OpenCLBufferBase");
			}
				
			cl_mem buf = clCreateBuffer(wcon.lock().get(), rwflag, size*sizeof(T), nullptr, &err);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clCreateBuffer");
			}

			buffer = MemObjectUniquePtr(buf);
				
			CommandQueueWeakPtr wque = context.GetQueue();
			if(wque.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLBufferBase::OpenCLBufferBase");					
			}

				
			err = clEnqueueCopyBuffer(wque.lock().get(), src.buffer.get(), buffer.get(), 0, 0, size*sizeof(T), 0, nullptr, nullptr);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clEnqueueCopyBuffer");
			}

			context.Synchronize();
		}
		




		unsigned int Size()
		{
			return size;
		}

		unsigned int MappedSize()
		{
			return mappedSize;
		}
	
		const cl_mem GetMemObject()
		{
			return buffer.get();
		}

		T* GetMappedPointer()
		{
			return this->mappedPointer;
		}	

		void SetReadWriteMode(unsigned int mode)
		{
			readWriteMode = mode;
		}
	
		virtual T* MapToHost(CommandQueueWeakPtr queue, size_t offset, size_t length) = 0;

		virtual T* MapToHost(CommandQueueWeakPtr queue) = 0;

		virtual T* MapToHost(size_t offset, size_t length) = 0;
		
		virtual T* MapToHost() = 0;

		virtual T& operator[](unsigned int i) = 0;
		virtual void Unmap() = 0;

		virtual OpenCLEvent MapToHost(CommandQueueWeakPtr queue, size_t offset, size_t length, OpenCLEventList &eventList) = 0;

		virtual OpenCLEvent MapToHost(CommandQueueWeakPtr queue, OpenCLEventList &eventList) = 0;

		virtual OpenCLEvent MapToHost(size_t offset, size_t length, OpenCLEventList &eventList) = 0;
		
		virtual OpenCLEvent MapToHost(OpenCLEventList &eventList) = 0;


		virtual OpenCLEvent Unmap(OpenCLEventList &eventList) = 0;				

		T* begin()
		{
			return mappedPointer;
		}

		T* end()
		{
			return mappedPointer + mappedSize;
		}


	};


	template<typename T>
	class OpenCLUnifiedBuffer : public OpenCLBufferBase<T>
	{
	
	public:

		OpenCLUnifiedBuffer(OpenCLContext &context, unsigned int size, cl_mem_flags rwflag) //rwflag: CL_MEM_READ_WRITE,...
			: OpenCLBufferBase<T>(context, size,
								  ((CL_MEM_READ_WRITE | CL_MEM_READ_ONLY | CL_MEM_WRITE_ONLY) & rwflag) | CL_MEM_ALLOC_HOST_PTR)
		{
		}


		//Blocking mode
		T* MapToHost(CommandQueueWeakPtr queue, size_t offset, size_t length)
		{
			cl_int err;

			if(offset >= this->size || (length + offset) > this->size)
			{
				throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLUnifiedBuffer::MapToHost");
			}
		
			if(this->mappedPointer != nullptr)
			{
				if(this->offset == offset && length == this->mappedSize)
					return this->mappedPointer;
				else
				{
					Unmap();
				}
			}

			if(queue.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLUnifiedBuffer::MapToHost");
			}

			
			this->mappedPointer = (T*)clEnqueueMapBuffer(queue.lock().get(), this->buffer.get(), CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, offset*sizeof(T), length*sizeof(T), 0, nullptr, nullptr, &err);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clEnqueueMapBuffer");
			}

			this->mappedSize = length;
			this->offset = offset;
			this->queueForUnmap = queue;

			return this->mappedPointer;
		}


		T* MapToHost(CommandQueueWeakPtr queue)
		{
			return MapToHost(queue, 0, this->size);
		}

		T* MapToHost(size_t offset, size_t length)
		{
			T* tp;

			CommandQueueWeakPtr queue = this->context.GetQueue(0);
			tp = MapToHost(queue, offset, length);

			return tp;
		}


		T* MapToHost()
		{
			
			T* tp;
			tp = MapToHost(0, this->size);
			return tp;
		}		

		void Unmap()
		{
			cl_int err;
			if(this->mappedPointer == nullptr) return;

			if(this->queueForUnmap.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLUnifiedBuffer::Unmap");					
			}
			err = clEnqueueUnmapMemObject(this->queueForUnmap.lock().get(), this->buffer.get(), this->mappedPointer, 0, nullptr, nullptr);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clEnqueueUnmapMemObject");
			}
			this->mappedPointer = nullptr;
			this->mappedSize = 0;
		}


		//Non-blocking mode
		OpenCLEvent MapToHost(CommandQueueWeakPtr queue, size_t offset, size_t length, OpenCLEventList &eventList)
		{
			cl_int err;
			cl_event event;

			if(offset >= this->size || (length + offset) > this->size)
			{
				throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLUnifiedBuffer::MapToHost");
			}
		
			if(this->mappedPointer != nullptr)
			{
				throw OpenCLException(CL_INVALID_EVENT, "OpenCLUnifiedBufer::MapToHost", "This object already mapped.");
			}
				/*
			{
				if(this->offset == offset && length == this->mappedSize)
					return this->mappedPointer;
				else
				{
					Unmap(eventList);
				}
				}*/

			if(queue.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLUnifiedBuffer::MapToHost");
			}

			
			this->mappedPointer = (T*)clEnqueueMapBuffer(queue.lock().get(), this->buffer.get(), CL_FALSE, CL_MAP_READ | CL_MAP_WRITE, offset*sizeof(T), length*sizeof(T), eventList.size(), eventList.GetList(), &event, &err);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clEnqueueMapBuffer");
			}

			this->mappedSize = length;
			this->offset = offset;
			this->queueForUnmap = queue;

			return std::move(OpenCLEvent(event));
		}


		OpenCLEvent MapToHost(CommandQueueWeakPtr queue, OpenCLEventList &eventList)
		{
			return std::move(MapToHost(queue, 0, this->size, eventList));
		}

		OpenCLEvent MapToHost(size_t offset, size_t length, OpenCLEventList &eventList)
		{

			CommandQueueWeakPtr queue = this->context.GetQueue(0);
			return std::move(MapToHost(queue, offset, length, eventList));
		}


		OpenCLEvent MapToHost(OpenCLEventList &eventList)
		{			
			return std::move(MapToHost(0, this->size, eventList));
		}		

		OpenCLEvent Unmap(OpenCLEventList &eventList)
		{
			cl_int err;
			cl_event event;
			if(this->mappedPointer == nullptr) return std::move(OpenCLEvent());

			if(this->queueForUnmap.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLUnifiedBuffer::Unmap");					
			}
			err = clEnqueueUnmapMemObject(this->queueForUnmap.lock().get(), this->buffer.get(), this->mappedPointer, eventList.size(), eventList.GetList(), &event);
			if(err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clEnqueueUnmapMemObject");
			}
			this->mappedPointer = nullptr;
			this->mappedSize = 0;

			return std::move(OpenCLEvent(event));
		}		



		T& operator[](unsigned int i)
		{
			if(this->mappedPointer == nullptr)
			{
				MapToHost();
			}
			return *(this->mappedPointer + i);
		}

		OpenCLUnifiedBuffer(const OpenCLUnifiedBuffer &src)
			: OpenCLBufferBase<T>(src)
								  
		{
			if(this->mappedPointer != nullptr)
			{
				this->mappedPointer = nullptr;
				MapToHost(this->offset, this->mappedSize);
			}
		}		
	};

	
	template<typename T>
	class OpenCLDeviceBuffer : public OpenCLBufferBase<T>
	{

		T* hostMemory;
	public:

		OpenCLDeviceBuffer(OpenCLContext &context, unsigned int size, cl_mem_flags rwflag) //rwflag: CL_MEM_READ_WRITE,...
			: OpenCLBufferBase<T>(context, size,
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


	
		//Blocking mode
		T* MapToHost(CommandQueueWeakPtr queue, size_t offset, size_t length)
		{
			cl_int err;
		
			if(offset >= this->size || (length + offset) > this->size)
			{
				throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLDeviceBuffer::MapToHost");
			}


			if(queue.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLDeviceBuffer::MapToHost");
			}
				
				
			if(this->readWriteMode & OPENCL_READ_BUFFER)
			{
				err = clEnqueueReadBuffer(queue.lock().get(), this->buffer, CL_TRUE, offset*sizeof(T), length*sizeof(T), hostMemory, 0 , nullptr, nullptr);
				if(err != CL_SUCCESS) throw OpenCLException(err, "clEnqueueReadBuffer");
			}
			this->mappedSize = length;
			this->offset = offset;
			this->mappedPointer = hostMemory + this->offset;
			this->queueForUnmap = queue;
		
			return this->mappedPointer;
		}	


		T* MapToHost(CommandQueueWeakPtr queue)
		{
			return MapToHost(queue, 0, this->size);
		}


		T* MapToHost(size_t offset, size_t length)
		{
			T* tp;

			CommandQueueWeakPtr queue = this->context.GetQueue(0);
			tp = MapToHost(queue, offset, length);
			return tp;
		}

		T* MapToHost()
		{
			return MapToHost(0, this->size);			
		}

		
		void Unmap()
		{
			cl_int err;
			if(this->mappedPointer == nullptr) return;


			if(this->queueForUnmap.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLDeviceBuffer::Unmap");
			}
			if(this->readWriteMode & OPENCL_WRITE_BUFFER)
			{
				err = clEnqueueWriteBuffer(this->queueForUnmap.lock().get(), this->buffer, CL_TRUE, this->offset*sizeof(T), this->mappedSize*sizeof(T), hostMemory, 0 , nullptr, nullptr);
				if(err != CL_SUCCESS) throw OpenCLException(err, "clEnqueueWriteBuffer");
			}			
			this->mappedPointer = nullptr;
			
		}

		//Non-blocking mode
		OpenCLEvent MapToHost(CommandQueueWeakPtr queue, size_t offset, size_t length, OpenCLEventList &eventList)
		{
			cl_int err;
			cl_event event;
		
			if(offset >= this->size || (length + offset) > this->size)
			{
				throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLDeviceBuffer::MapToHost");
			}


			if(queue.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLDeviceBuffer::MapToHost");
			}
				
				
			if(this->readWriteMode & OPENCL_READ_BUFFER)
			{
				err = clEnqueueReadBuffer(queue.lock().get(), this->buffer, CL_FALSE, offset*sizeof(T), length*sizeof(T), hostMemory, eventList.size(), eventList.GetList(), &event);
				if(err != CL_SUCCESS) throw OpenCLException(err, "clEnqueueReadBuffer");
			}
			this->mappedSize = length;
			this->offset = offset;
			this->mappedPointer = hostMemory + this->offset;
			this->queueForUnmap = queue;
		
			return std::move(OpenCLEvent(event));
		}	


		OpenCLEvent MapToHost(CommandQueueWeakPtr queue, OpenCLEventList &eventList)
		{
			return MapToHost(queue, 0, this->size, eventList);
		}



		OpenCLEvent MapToHost(size_t offset, size_t length, OpenCLEventList &eventList)
		{
			CommandQueueWeakPtr queue = this->context.GetQueue(0);
			return std::move(MapToHost(queue, offset, length, eventList));

		}

		OpenCLEvent MapToHost(OpenCLEventList &eventList)
		{
			return std::move(MapToHost(0, this->size, eventList));			
		}

		
		OpenCLEvent Unmap(OpenCLEventList &eventList)
		{
			cl_int err;
			cl_event event;
			if(this->mappedPointer == nullptr) return std::move(OpenCLEvent());


			if(this->queueForUnmap.expired())
			{
				throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLDeviceBuffer::Unmap");
			}
			if(this->readWriteMode & OPENCL_WRITE_BUFFER)
			{
				err = clEnqueueWriteBuffer(this->queueForUnmap.lock().get(), this->buffer, CL_FALSE, this->offset*sizeof(T), this->mappedSize*sizeof(T), hostMemory, eventList.size(), eventList.GetList(), &event);
				if(err != CL_SUCCESS) throw OpenCLException(err, "clEnqueueWriteBuffer");
			}			
			this->mappedPointer = nullptr;

			return std::move(OpenCLEvent(event));
		}		



		T& operator[](unsigned int i)
		{
			if(this->mappedPointer == nullptr)
			{
				MapToHost();
			}
			return *(this->mappedPointer + i);
		}

		OpenCLDeviceBuffer(const OpenCLDeviceBuffer &src)
			: OpenCLBufferBase<T>(src)
								  
		{
			try
			{
				hostMemory = new T[this->size];
			}
			catch(std::bad_alloc e)
			{
				throw OpenCLException(CL_OUT_OF_HOST_MEMORY, "OpenCLDeviceBuffer::OpenCLDeviceBuffer");
			}
			
			if(this->mappedPointer != nullptr)
			{
				//MapToHost(this->offset, this->length);
				this->mappedPointer = this->hostMemory;
				std::copy(src.hostMemory, src.hostMemory + src.size, this->hostMemory);
			}
		}				
	};


}


#endif
