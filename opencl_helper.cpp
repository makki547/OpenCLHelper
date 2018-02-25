#include "opencl_helper.hpp"

std::string opencl_helper::OpenCLException::GetErrorName(cl_int err)
{

	std::unordered_map<cl_int, std::string> errorCodes
	{
		{CL_SUCCESS, std::string("CL_SUCCESS")},
		{CL_DEVICE_NOT_FOUND, std::string("CL_DEVICE_NOT_FOUND")},
		{CL_DEVICE_NOT_AVAILABLE, std::string("CL_DEVICE_NOT_AVAILABLE")},
		{CL_COMPILER_NOT_AVAILABLE, std::string("CL_COMPILER_NOT_AVAILABLE")},
		{CL_MEM_OBJECT_ALLOCATION_FAILURE, std::string("CL_MEM_OBJECT_ALLOCATION_FAILURE")},
		{CL_OUT_OF_RESOURCES, std::string("CL_OUT_OF_RESOURCES")},
		{CL_OUT_OF_HOST_MEMORY, std::string("CL_OUT_OF_HOST_MEMORY")},
		{CL_PROFILING_INFO_NOT_AVAILABLE, std::string("CL_PROFILING_INFO_NOT_AVAILABLE")},
		{CL_MEM_COPY_OVERLAP, std::string("CL_MEM_COPY_OVERLAP")},
		{CL_IMAGE_FORMAT_MISMATCH, std::string("CL_IMAGE_FORMAT_MISMATCH")},
		{CL_IMAGE_FORMAT_NOT_SUPPORTED, std::string("CL_IMAGE_FORMAT_NOT_SUPPORTED")},
		{CL_BUILD_PROGRAM_FAILURE, std::string("CL_BUILD_PROGRAM_FAILURE")},
		{CL_MAP_FAILURE, std::string("CL_MAP_FAILURE")},
		{CL_MISALIGNED_SUB_BUFFER_OFFSET, std::string("CL_MISALIGNED_SUB_BUFFER_OFFSET")},
		{CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST, std::string("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST")},
		{CL_COMPILE_PROGRAM_FAILURE, std::string("CL_COMPILE_PROGRAM_FAILURE")},
		{CL_LINKER_NOT_AVAILABLE, std::string("CL_LINKER_NOT_AVAILABLE")},
		{CL_LINK_PROGRAM_FAILURE, std::string("CL_LINK_PROGRAM_FAILURE")},
		{CL_DEVICE_PARTITION_FAILED, std::string("CL_DEVICE_PARTITION_FAILED")},
		{CL_KERNEL_ARG_INFO_NOT_AVAILABLE, std::string("CL_KERNEL_ARG_INFO_NOT_AVAILABLE")},
		{CL_INVALID_VALUE, std::string("CL_INVALID_VALUE")},
		{CL_INVALID_DEVICE_TYPE, std::string("CL_INVALID_DEVICE_TYPE")},
		{CL_INVALID_PLATFORM, std::string("CL_INVALID_PLATFORM")},
		{CL_INVALID_DEVICE, std::string("CL_INVALID_DEVICE")},
		{CL_INVALID_CONTEXT, std::string("CL_INVALID_CONTEXT")},
		{CL_INVALID_QUEUE_PROPERTIES, std::string("CL_INVALID_QUEUE_PROPERTIES")},
		{CL_INVALID_COMMAND_QUEUE, std::string("CL_INVALID_COMMAND_QUEUE")},
		{CL_INVALID_HOST_PTR, std::string("CL_INVALID_HOST_PTR")},
		{CL_INVALID_MEM_OBJECT, std::string("CL_INVALID_MEM_OBJECT")},
		{CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, std::string("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR")},
		{CL_INVALID_IMAGE_SIZE, std::string("CL_INVALID_IMAGE_SIZE")},
		{CL_INVALID_SAMPLER, std::string("CL_INVALID_SAMPLER")},
		{CL_INVALID_BINARY, std::string("CL_INVALID_BINARY")},
		{CL_INVALID_BUILD_OPTIONS, std::string("CL_INVALID_BUILD_OPTIONS")},
		{CL_INVALID_PROGRAM, std::string("CL_INVALID_PROGRAM")},
		{CL_INVALID_PROGRAM_EXECUTABLE, std::string("CL_INVALID_PROGRAM_EXECUTABLE")},
		{CL_INVALID_KERNEL_NAME, std::string("CL_INVALID_KERNEL_NAME")},
		{CL_INVALID_KERNEL_DEFINITION, std::string("CL_INVALID_KERNEL_DEFINITION")},
		{CL_INVALID_KERNEL, std::string("CL_INVALID_KERNEL")},
		{CL_INVALID_ARG_INDEX, std::string("CL_INVALID_ARG_INDEX")},
		{CL_INVALID_ARG_VALUE, std::string("CL_INVALID_ARG_VALUE")},
		{CL_INVALID_ARG_SIZE, std::string("CL_INVALID_ARG_SIZE")},
		{CL_INVALID_KERNEL_ARGS, std::string("CL_INVALID_KERNEL_ARGS")},
		{CL_INVALID_WORK_DIMENSION, std::string("CL_INVALID_WORK_DIMENSION")},
		{CL_INVALID_WORK_GROUP_SIZE, std::string("CL_INVALID_WORK_GROUP_SIZE")},
		{CL_INVALID_WORK_ITEM_SIZE, std::string("CL_INVALID_WORK_ITEM_SIZE")},
		{CL_INVALID_GLOBAL_OFFSET, std::string("CL_INVALID_GLOBAL_OFFSET")},
		{CL_INVALID_EVENT_WAIT_LIST, std::string("CL_INVALID_EVENT_WAIT_LIST")},
		{CL_INVALID_EVENT, std::string("CL_INVALID_EVENT")},
		{CL_INVALID_OPERATION, std::string("CL_INVALID_OPERATION")},
		{CL_INVALID_GL_OBJECT, std::string("CL_INVALID_GL_OBJECT")},
		{CL_INVALID_BUFFER_SIZE, std::string("CL_INVALID_BUFFER_SIZE")},
		{CL_INVALID_MIP_LEVEL, std::string("CL_INVALID_MIP_LEVEL")},
		{CL_INVALID_GLOBAL_WORK_SIZE, std::string("CL_INVALID_GLOBAL_WORK_SIZE")},
		{CL_INVALID_PROPERTY, std::string("CL_INVALID_PROPERTY")},
		{CL_INVALID_IMAGE_DESCRIPTOR, std::string("CL_INVALID_IMAGE_DESCRIPTOR")},
		{CL_INVALID_COMPILER_OPTIONS, std::string("CL_INVALID_COMPILER_OPTIONS")},
		{CL_INVALID_LINKER_OPTIONS, std::string("CL_INVALID_LINKER_OPTIONS")},
		{CL_INVALID_DEVICE_PARTITION_COUNT, std::string("CL_INVALID_DEVICE_PARTITION_COUNT")}
	};
	
	std::string error;
	try
	{
		error = errorCodes[err];
	}
	catch(std::out_of_range e)
	{
		error = std::string("Unknown error");
	}
	return error;	
}

std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLException& exception)
{
	if(exception.errorDetail.empty())
	{
		os << "Function name: " << exception.errorFunction << ", Error: " << exception.errorName << "(" << exception.err << ")";
	}
	else
	{
		os << "Function name: " << exception.errorFunction << "(" << exception.errorDetail  << "), Error: " << exception.errorName << "(" << exception.err << ")";		

	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLEvent& event)
{
	std::unordered_map<cl_command_type, std::string> commandTypeCodes
	{
		{ CL_COMMAND_NDRANGE_KERNEL, std::string("CL_COMMAND_NDRANGE_KERNEL") },
		{ CL_COMMAND_TASK, std::string("CL_COMMAND_TASK") },
		{ CL_COMMAND_NATIVE_KERNEL, std::string("CL_COMMAND_NATIVE_KERNEL") },
		{ CL_COMMAND_READ_BUFFER, std::string("CL_COMMAND_READ_BUFFER") },
		{ CL_COMMAND_WRITE_BUFFER, std::string("CL_COMMAND_WRITE_BUFFER") },
		{ CL_COMMAND_COPY_BUFFER, std::string("CL_COMMAND_COPY_BUFFER") },
		{ CL_COMMAND_READ_IMAGE, std::string("CL_COMMAND_READ_IMAGE") },
		{ CL_COMMAND_WRITE_IMAGE, std::string("CL_COMMAND_WRITE_IMAGE") },
		{ CL_COMMAND_COPY_IMAGE, std::string("CL_COMMAND_COPY_IMAGE") },
		{ CL_COMMAND_COPY_IMAGE_TO_BUFFER, std::string("CL_COMMAND_COPY_IMAGE_TO_BUFFER") },
		{ CL_COMMAND_COPY_BUFFER_TO_IMAGE, std::string("CL_COMMAND_COPY_BUFFER_TO_IMAGE") },
		{ CL_COMMAND_MAP_BUFFER, std::string("CL_COMMAND_MAP_BUFFER") },
		{ CL_COMMAND_MAP_IMAGE, std::string("CL_COMMAND_MAP_IMAGE") },
		{ CL_COMMAND_UNMAP_MEM_OBJECT, std::string("CL_COMMAND_UNMAP_MEM_OBJECT") },
		{ CL_COMMAND_MARKER, std::string("CL_COMMAND_MARKER") },
		{ CL_COMMAND_ACQUIRE_GL_OBJECTS, std::string("CL_COMMAND_ACQUIRE_GL_OBJECTS") },
		{ CL_COMMAND_RELEASE_GL_OBJECTS, std::string("CL_COMMAND_RELEASE_GL_OBJECTS") },
		{ CL_COMMAND_READ_BUFFER_RECT, std::string("CL_COMMAND_READ_BUFFER_RECT") },
		{ CL_COMMAND_WRITE_BUFFER_RECT, std::string("CL_COMMAND_WRITE_BUFFER_RECT") },
		{ CL_COMMAND_COPY_BUFFER_RECT, std::string("CL_COMMAND_COPY_BUFFER_RECT") },
		{ CL_COMMAND_USER, std::string("CL_COMMAND_USER") },
		{ CL_COMMAND_BARRIER, std::string("CL_COMMAND_BARRIER") },
		{ CL_COMMAND_MIGRATE_MEM_OBJECTS, std::string("CL_COMMAND_MIGRATE_MEM_OBJECTS") },
		{ CL_COMMAND_FILL_BUFFER, std::string("CL_COMMAND_FILL_BUFFER") },
		{ CL_COMMAND_FILL_IMAGE, std::string("CL_COMMAND_FILL_IMAGE") }
#ifdef OCL_HELPER_V2_SUPPORT
		,
		{ CL_COMMAND_SVM_FREE, std::string("CL_COMMAND_SVM_FREE") },
		{ CL_COMMAND_SVM_MEMCPY, std::string("CL_COMMAND_SVM_MEMCPY") },
		{ CL_COMMAND_SVM_MEMFILL, std::string("CL_COMMAND_SVM_MEMFILL") },
		{ CL_COMMAND_SVM_MAP, std::string("CL_COMMAND_SVM_MAP") },
		{ CL_COMMAND_SVM_UNMAP, std::string("CL_COMMAND_SVM_UNMAP") }
#endif
	};

	std::unordered_map<cl_int, std::string> statusCodes
	{
		{ CL_COMPLETE, std::string("Complete")},
		{ CL_RUNNING, std::string("Running") },
		{ CL_SUBMITTED, std::string("Submitted") },
		{ CL_QUEUED, std::string("Queued") },
	};

	std::string cmd;
	std::string status;
	const_cast<opencl_helper::OpenCLEvent&>(event).GetEventCommandType();
	const_cast<opencl_helper::OpenCLEvent&>(event).GetStatus();

	try
	{
		cmd = commandTypeCodes.at(event.cmdType);
	}
	catch (std::out_of_range exception)
	{
		cmd = std::string("Unknown");
	}

	try
	{
		status = statusCodes.at(event.status);
	}
	catch (std::out_of_range exception)
	{
		status = std::string("Unknown");
	}

	os << "Command: " << cmd << ", Status: " << status;
	return os;
}

std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLEvents& events)
{
	for (auto itr = events.begin(); itr != events.end(); ++itr)
	{
		os << "Name: " << itr->first << ", " << itr->second << std::endl;
	}
	return os;
}

opencl_helper::OpenCLDevice::OpenCLDevice(cl_device_id deviceID)
	: deviceID(deviceID)
{
	available = GetInformation<cl_bool>(CL_DEVICE_AVAILABLE);
	hostUnifiedMemory = GetInformation<cl_bool>(CL_DEVICE_HOST_UNIFIED_MEMORY);
	vendor = GetInformation(CL_DEVICE_VENDOR);
	name = GetInformation(CL_DEVICE_NAME);
	version = GetInformation(CL_DEVICE_VERSION);
#ifdef OCL_HELPER_V2_SUPPORT
	svmCapabilities = GetInformation<cl_device_svm_capabilities>(CL_DEVICE_SVM_CAPABILITIES);
#endif
		  
}

opencl_helper::OpenCLDeviceIDList opencl_helper::OpenCLDevices::GetDeviceIDList()
{
	OpenCLDeviceIDList devlist;
	devlist.reserve(size());
	
	for(auto itr = begin();itr != end();++itr)
	{
		if(*itr)
		{
			devlist.push_back(itr->GetDeviceID());
		}
	}
	
	return std::move(devlist);
}

std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLDevice& device)
{
	os << "Device name: " << device.name << std::endl;
	os << "Vendor: " << device.vendor << std::endl;
	os << "Version: " << device.version << std::endl;

#ifdef OCL_HELPER_V2_SUPPORT



	os << "Unified memory (ALLOC_HOST_PTR) available: " << (device.hostUnifiedMemory ? "Yes" : "No") << std::endl;
	os << "SVM Capabilities: ";
	if (device.svmCapabilities)
	{
		if (device.svmCapabilities & CL_DEVICE_SVM_FINE_GRAIN_SYSTEM)
		{
			os << "Fine grain system";
		}
		else if (device.svmCapabilities & CL_DEVICE_SVM_FINE_GRAIN_BUFFER)
		{
			os << "Fine grain buffer";
		}
		else if (device.svmCapabilities & CL_DEVICE_SVM_COARSE_GRAIN_BUFFER)
		{
			os << "Coarse grain buffer";
		}
		os << std::endl;

		os << "\tMemory allocation: ";
		if (device.svmCapabilities & CL_DEVICE_SVM_FINE_GRAIN_SYSTEM)
		{
			os << "Malloc";
		}
		else
		{
			os << "clSVMalloc";
		}
		os << std::endl;
		os << "\tMemory consistency: ";
		if (device.svmCapabilities & CL_DEVICE_SVM_FINE_GRAIN_BUFFER)
		{
			os << "Guaranteed";
		}
		else
		{
			os << "No, Use map/unmap";
		}
		os << std::endl;

		os << "\tSupport atomic operations: " << ((device.svmCapabilities & CL_DEVICE_SVM_ATOMICS) ? "Yes" : "No") << std::endl;

	}
	else
	{
		os << "No" << std::endl;
	}

#else
	os << "Unified memory available: " << (device.hostUnifiedMemory ? "Yes" : "No") << std::endl;
#endif
	os << std::endl;
	return os;
}


std::vector<std::string> opencl_helper::OpenCLPlatformInformations::GetVendors()
{
	std::vector<std::string> vendors;
	vendors.reserve(size());
	for(auto itr = begin();itr != end();++itr)
	{
		vendors.push_back(itr->vendor);
	}
	return std::move(vendors);
}

std::vector<std::string> opencl_helper::OpenCLPlatformInformations::GetVersions()
{
	std::vector<std::string> versions;
	versions.reserve(size());
	for(auto itr = begin();itr != end();++itr)
	{
		versions.push_back(itr->version);
	}
	return std::move(versions);
}

opencl_helper::OpenCLPlatforms::OpenCLPlatforms(cl_device_type openclDeviceType)
{
	cl_int err;

	cl_platform_id platforms[PLATFORM_MAX];
	cl_device_id devices[DEVICE_MAX];
	char vendor[100] = {0};
	char version[100] = {0};
	char deviceName[100] = {0};
	cl_uint nplatforms;
	cl_uint ndevices;
	size_t reslen;
		
	err = clGetPlatformIDs(PLATFORM_MAX, platforms, &nplatforms);
	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clGetPlatformIDs");
	}
	if(nplatforms == 0)
	{
		throw OpenCLException(err, "clGetPlatformIDs", "No platforms");			
	}

	platformInformations.resize(nplatforms);
	for(cl_uint i = 0;i < nplatforms;i++)
	{
		platformInformations[i].platformID = platforms[i];

        
		err = clGetPlatformInfo(platformInformations[i].platformID, CL_PLATFORM_VENDOR, sizeof(vendor), vendor, 0);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetPlatformInfo", "CL_PLATFORM_VENDOR");

		platformInformations[i].vendor = std::string(vendor);

		err = clGetPlatformInfo(platformInformations[i].platformID, CL_PLATFORM_VERSION, sizeof(version), version, 0);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetPlatformInfo", "CL_PLATFORM_VERSION");
		platformInformations[i].version = std::string(version);

			
		err = clGetDeviceIDs(platformInformations[i].platformID, openclDeviceType, DEVICE_MAX, devices, &ndevices);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetDeviceIDs");

		platformInformations[i].devices.reserve(ndevices);
			
		for(cl_uint j = 0;j < ndevices;j++)
		{
			OpenCLDevice dev(devices[j]);
			if(dev)
			{
				platformInformations[i].devices.push_back(dev);
			}
		}

	}
	
}


unsigned int opencl_helper::OpenCLPlatforms::CountDevices(unsigned int platformIndex)
{
	unsigned int n = 0;
	try
	{
		n = platformInformations.at(platformIndex).devices.size();
	}
	catch(std::out_of_range e)
	{
		throw OpenCLException(CL_INVALID_PLATFORM, "OpenCLPlatforms::CountDevices", "Invalid platform index");
	}
	return n;
}


opencl_helper::OpenCLDevices opencl_helper::OpenCLPlatforms::GetDeviceList(unsigned int platformIndex, std::vector<unsigned int> deviceIndices)
{
	OpenCLDevices devices;


	try
	{
		devices.reserve(deviceIndices.size());

		for(auto itr = deviceIndices.begin();itr != deviceIndices.end();++itr)
		{
			devices.push_back(platformInformations.at(platformIndex).devices.at(*itr).GetDeviceID());
		}
	}
	catch(std::out_of_range e)
	{
		throw OpenCLException(CL_INVALID_PLATFORM, "OpenCLPlatforms::GetDeviceList", "Invalid platform and/or device index");				
	}

	return std::move(devices);
}

opencl_helper::OpenCLDevices opencl_helper::OpenCLPlatforms::GetDeviceList(unsigned int platformIndex)
{

	OpenCLDevices devices;

	try
	{
		std::vector<unsigned int> deviceIndices(platformInformations.at(platformIndex).devices.size());
		std::iota(deviceIndices.begin(), deviceIndices.end(), 0);
		devices.reserve(deviceIndices.size());

		devices = GetDeviceList(platformIndex, deviceIndices);
				
	}
	catch(std::out_of_range e)
	{
		throw OpenCLException(CL_INVALID_PLATFORM, "OpenCLPlatforms::GetDeviceList", "Invalid platform index");				
	}
	
	return std::move(devices);
}		

std::ostream& operator<<(std::ostream& os, const opencl_helper::OpenCLPlatforms& platforms)
{

	unsigned int platformID = 0;
	for(auto itr = platforms.platformInformations.begin(); itr != platforms.platformInformations.end();++itr)
	{
		os << "Platform index: " << platformID++ << std::endl;
		os << std::endl;
		os << "Platform vendor: " <<  itr->vendor << std::endl;
		os << "Platform version: " <<  itr->version << std::endl;
		os << std::endl;

		unsigned int deviceID = 0;
		for(auto dev = itr->devices.begin();dev != itr->devices.end();++dev)
		{
			os << "Device index: " << deviceID++ << std::endl;
			os << std::endl;			
			os << *dev;
		}

		os << "----------------------------------------------------" << std::endl;
		os << std::endl;
		os << std::endl;
			
	}
	return os;
}

opencl_helper::OpenCLDeviceQueue::OpenCLDeviceQueue(const opencl_helper::OpenCLDevice &device, opencl_helper::ContextWeakPtr context, bool enableOutOfOrderMode)
	: device(device)
{
	cl_int err;


	if(context.expired())
	{
		throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLDeviceQueue::OpenCLDeviceQueue");
	}
	ContextSharedPtr con = context.lock();

	cl_bitfield mode = enableOutOfOrderMode ? CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE : 0;
	
	cl_command_queue q = clCreateCommandQueue(con.get(), OpenCLDeviceQueue::device.GetDeviceID(), mode, &err);
	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clCreateCommandQueue");
	}
	queue = CommandQueueSharedPtr(q, OpenCLReleaseQueue());

}

opencl_helper::OpenCLContext::OpenCLContext(const opencl_helper::OpenCLDevices& devices, bool enableOutOfOrderMode)
	: devices(devices)
{
	cl_int err;

	if(devices.empty())
	{
		throw OpenCLException(CL_INVALID_DEVICE, "OpenCLContext::OpenCLContext", "Device list is empty.");
	}
	OpenCLDeviceIDList deviceList = OpenCLContext::devices.GetDeviceIDList();
	cl_context con = clCreateContext(nullptr, deviceList.size(), &(deviceList[0]), nullptr, nullptr, &err);

	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clCreateContext");
	}

	context = std::move(ContextSharedPtr(con, OpenCLReleaseContext()));

	queues.reserve(devices.size());
	unifiedMemoryAvailable = false;
#ifdef OCL_HELPER_V2_SUPPORT
	svmCapabilities = 0;
#endif
	for(auto itr = OpenCLContext::devices.begin();itr != OpenCLContext::devices.end();++itr)
	{
		queues.push_back(OpenCLDeviceQueue(*itr, context, enableOutOfOrderMode));
		unifiedMemoryAvailable |= itr->UnifiedMemoryAvailable();

#ifdef OCL_HELPER_V2_SUPPORT
		svmCapabilities |= itr->SVMCapabilities();
#endif
	}
}


opencl_helper::CommandQueueWeakPtr opencl_helper::OpenCLContext::GetQueue(unsigned int deviceIndex)
{
	CommandQueueWeakPtr p;
	try
	{
		p = queues.at(deviceIndex).GetQueue();
	}
	catch(std::out_of_range exception)
	{
		throw OpenCLException(CL_INVALID_DEVICE, "OpenCLContext::GetQueue");
	}
	return std::move(p);
}

void opencl_helper::OpenCLContext::WaitDevice(unsigned int deviceIndex)
{
	cl_int err;
	try
	{
		CommandQueueWeakPtr p = queues.at(deviceIndex).GetQueue();
		if(p.expired())
		{
			throw OpenCLException(CL_INVALID_DEVICE, "OpenCLContext::WaitDevice");
		}
		err = clFinish(p.lock().get());
		if(err != CL_SUCCESS) throw OpenCLException(err, "clFinish");									
	}
	catch(OpenCLException exception)
	{
		throw exception;
	}
	catch(std::out_of_range oor)
	{
		throw OpenCLException(CL_INVALID_COMMAND_QUEUE, "clFinish", "Invalid device index");
	}
}

void opencl_helper::OpenCLContext::Synchronize()
{
	cl_int err;

	for(auto itr = queues.begin();itr != queues.end();++itr)
	{
		CommandQueueWeakPtr p = itr->GetQueue();
		if(!p.expired())
		{
			err = clFinish(p.lock().get());					
		}
		if(err != CL_SUCCESS) throw OpenCLException(err, "clFinish");					
	}				
}

opencl_helper::OpenCLProgram::OpenCLProgram(OpenCLContext &context, std::string kernelFileName)
	: context(&context)
{

	cl_int err;


	try
	{
		std::ifstream ifs(kernelFileName.c_str());
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::istreambuf_iterator<char> kernelBeginItr(ifs);
		std::istreambuf_iterator<char> kernelEndItr;
		std::string kernelSource(kernelBeginItr, kernelEndItr);
		
		size_t kernelLength = kernelSource.size();
		const char* kernelSourcePointer  = kernelSource.c_str();

		ContextWeakPtr wcon = OpenCLProgram::context->GetContext();

		if(wcon.expired())
		{
			throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLProgram::OpenCLProgram");
		}

		cl_program p = clCreateProgramWithSource(wcon.lock().get(), 1, &kernelSourcePointer, &kernelLength, &err);

		if(err != CL_SUCCESS) throw OpenCLException(err, "clCreateProgramWithSource");

		program = ProgramSharedPtr(p, OpenCLReleaseProgram());
		OpenCLDeviceIDList devlist = OpenCLProgram::context->GetDeviceIDList();
		err = clBuildProgram(program.get(), devlist.size(), &(devlist[0]),
#ifdef OCL_HELPER_V2_SUPPORT
			"-cl-std=CL2.0",
#else
			nullptr,
#endif
			nullptr, nullptr);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clBuildProgram");
	}
	catch(OpenCLException exception)
	{
		char log[1024];
		
		if(err == CL_BUILD_PROGRAM_FAILURE)
		{
			cl_int err = clGetProgramBuildInfo(program.get(), OpenCLProgram::context->GetDeviceIDList()[0], CL_PROGRAM_BUILD_LOG, 1024, log, nullptr);
			if(err == CL_SUCCESS)
			{
				std::cerr << log << std::endl;
			}
			else
			{
				std::cerr << OpenCLException(err, "clGetProgramBuild", "CL_PROGRAM_BUILD_LOG") << std::endl;
			}
			program.reset();
			
		}
		throw exception;
	}
	catch(std::ifstream::failure e)
	{
		throw OpenCLException(CL_INVALID_KERNEL, "OpenCLProgram::OpenCLProgram");
	}	
	
}



opencl_helper::OpenCLKernels opencl_helper::OpenCLProgram::GetKernels()
{
	cl_int err;
	OpenCLKernels openclKernels;
	constexpr cl_uint NUM_MAX_KERNELS = 1024;
	cl_kernel kernels[NUM_MAX_KERNELS];
	cl_uint nkernels = 0;


	err = clCreateKernelsInProgram(program.get(), NUM_MAX_KERNELS, kernels, &nkernels);
	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clCreateKernelsInProgram");
	}
		
	for(cl_uint i = 0;i < nkernels;i++)
	{
		OpenCLKernel k(*this, kernels[i]);
		openclKernels.emplace(k.GetName(), k);
	}


	return openclKernels;
}



opencl_helper::OpenCLKernel::OpenCLKernel(OpenCLProgram& program, std::string name)
	: /*program(&program),*/ name(name)
{

	cl_int err;

	//ProgramWeakPtr wprog = program.GetProgram();
	context = program.GetContext();
	OpenCLKernel::program = program.GetProgram();
	if(OpenCLKernel::program.expired())
	{
		throw OpenCLException(CL_INVALID_PROGRAM, "OpenCLKernel::OpenCLKernel", name.c_str());			
	}
			
		
	cl_kernel ker = clCreateKernel(OpenCLKernel::program.lock().get(), name.c_str(), &err);
		
	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clCreateKernel", name.c_str());
	}

		
	kernel = KernelUniquePtr(ker);		
	cl_uint nargs;
	err = clGetKernelInfo(kernel.get(), CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &nargs, nullptr);
	if(err != CL_SUCCESS)
	{
		kernel.reset();
		throw OpenCLException(err, "clGetKernelInfo", name.c_str());
	}
	arguments.resize(nargs, Argument());

}

opencl_helper::OpenCLKernel::OpenCLKernel(OpenCLProgram& program, cl_kernel kernel)
	: /*program(&program),*/ kernel(KernelUniquePtr(kernel))
{

	constexpr size_t KERNEL_NAME_SIZE = 1024;

	cl_int err;		
	cl_uint nargs;
	char kernelName[KERNEL_NAME_SIZE];
	size_t nameLength = KERNEL_NAME_SIZE;
	context = program.GetContext();
	OpenCLKernel::program = program.GetProgram();
	err = clGetKernelInfo(OpenCLKernel::kernel.get(), CL_KERNEL_FUNCTION_NAME, sizeof(kernelName), kernelName, &nameLength);

	name = std::string(kernelName);
	if(err != CL_SUCCESS)
	{
		OpenCLKernel::kernel.reset();
		throw OpenCLException(err, "clGetKernelInfo", kernelName);
	}

		
	err = clGetKernelInfo(OpenCLKernel::kernel.get(), CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &nargs, nullptr);
	if(err != CL_SUCCESS)
	{
		OpenCLKernel::kernel.reset();			
		throw OpenCLException(err, "clGetKernelInfo", name.c_str());
	}
	arguments.resize(nargs, Argument());

}

opencl_helper::OpenCLKernel::OpenCLKernel(const OpenCLKernel &openclKernel)
	: name(openclKernel.name), program(openclKernel.program), arguments(openclKernel.arguments), context(openclKernel.context)
#ifdef OCL_HELPER_V2_SUPPORT
	, svmPointers(openclKernel.svmPointers)
#endif
{

	cl_int err;

	//ProgramWeakPtr wprog = OpenCLKernel::program->GetProgram();
	if(program.expired())
	{
		throw OpenCLException(CL_INVALID_PROGRAM, "OpenCLKernel::OpenCLKernel", name.c_str());			
	}		
	cl_kernel ker = clCreateKernel(program.lock().get(), name.c_str(), &err);
	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clCreateKernel", name.c_str());
	}
	kernel = KernelUniquePtr(ker);

	for(unsigned int i = 0;i < arguments.size();i++)
	{
		if(!arguments[i].size) continue;
#ifdef OCL_HELPER_V2_SUPPORT
		if (arguments[i].svm)
		{
			err = clSetKernelArgSVMPointer(kernel.get(), (cl_uint)i,  (void*)arguments[i].address);
		}
		else
		{
#endif
			if (arguments[i].address == nullptr)
			{
				if (arguments[i].buffer.expired())
				{
					throw OpenCLException(CL_INVALID_MEM_OBJECT, "OpenCLKernel::OpenCLKernel");
				}
				cl_mem mem = arguments[i].buffer.lock().get();
				err = clSetKernelArg(kernel.get(), (cl_uint)i, sizeof(cl_mem), (void*)&mem);
			}
			else
			{
				err = clSetKernelArg(kernel.get(), (cl_uint)i, arguments[i].size, (void*)arguments[i].address);
			}
		
#ifdef OCL_HELPER_V2_SUPPORT
		}
#endif

		if(err != CL_SUCCESS)
		{

			throw OpenCLException(err, "clSetKernelArg", name.c_str());
		}

#ifdef OCL_HELPER_V2_SUPPORT
		if (!svmPointers.empty())
		{
			err = clSetKernelExecInfo(kernel.get(), CL_KERNEL_EXEC_INFO_SVM_PTRS,
				svmPointers.size() * sizeof(void*), svmPointers.GetList());

			if (err != CL_SUCCESS)
			{
				throw OpenCLException(err, "clSetKernelExecInfo");
			}
		}
#endif
	}
	
}

opencl_helper::OpenCLKernel& opencl_helper::OpenCLKernel::operator=(const opencl_helper::OpenCLKernel &src)
{
	if(this == &src)
	{
		return *this;
	}


	program = src.program;
	arguments = src.arguments;
	name = src.name;
	context = src.context;
#ifdef OCL_HELPER_V2_SUPPORT
	svmPointers = src.svmPointers;
#endif

	

	cl_int err;
	
	//ProgramWeakPtr wprog = program->GetProgram();
	if(program.expired())
	{
		throw OpenCLException(CL_INVALID_PROGRAM, "OpenCLKernel::operator=", name.c_str());			
	}		
	cl_kernel ker = clCreateKernel(program.lock().get(), name.c_str(), &err);
	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clCreateKernel", name.c_str());
	}
	kernel = KernelUniquePtr(ker);

	for (unsigned int i = 0; i < arguments.size(); i++)
	{
		if (!arguments[i].size) continue;
#ifdef OCL_HELPER_V2_SUPPORT
		if (OpenCLKernel::arguments[i].svm)
		{
			err = clSetKernelArgSVMPointer(kernel.get(), (cl_uint)i, (void*)arguments[i].address);
		}
		else
		{
#endif
			if (arguments[i].address == nullptr)
			{
				if (arguments[i].buffer.expired())
				{
					throw OpenCLException(CL_INVALID_MEM_OBJECT, "OpenCLKernel::OpenCLKernel");
				}
				cl_mem mem = arguments[i].buffer.lock().get();
				err = clSetKernelArg(kernel.get(), (cl_uint)i, sizeof(cl_mem), (void*)&mem);
			}
			else
			{
				err = clSetKernelArg(kernel.get(), (cl_uint)i, arguments[i].size, (void*)arguments[i].address);
			}

#ifdef OCL_HELPER_V2_SUPPORT
		}
#endif

		if (err != CL_SUCCESS)
		{
			throw OpenCLException(err, "clSetKernelArg", name.c_str());
		}
	}

#ifdef OCL_HELPER_V2_SUPPORT
	if (!svmPointers.empty())
	{
		err = clSetKernelExecInfo(kernel.get(), CL_KERNEL_EXEC_INFO_SVM_PTRS,
			svmPointers.size() * sizeof(void*), svmPointers.GetList());

		if (err != CL_SUCCESS)
		{
			throw OpenCLException(err, "clSetKernelExecInfo");
		}
	}

#endif
	
	return *this;
}


opencl_helper::OpenCLEvent opencl_helper::OpenCLKernel::Execute(opencl_helper::CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize, const std::vector<size_t> &localWorkSize)
{
	cl_event event;
	if(ndim == 0 || 4 <= ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}
	if(globalWorkSize.size() != ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}
	if(localWorkSize.size() != ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}


	if(queue.expired())
	{
		throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLKernel::Execute");
	}
		
	cl_int err = clEnqueueNDRangeKernel(queue.lock().get(), kernel.get(), ndim, nullptr, globalWorkSize.data(), localWorkSize.data(), 0, nullptr, &event);

	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clEnqueueNDRangeKernel", name.c_str());
	}


	return std::move(OpenCLEvent(event));	
}

opencl_helper::OpenCLEvent opencl_helper::OpenCLKernel::Execute(opencl_helper::CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize)
{
	cl_event event;

	if(ndim == 0 || 4 <= ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}
	if(globalWorkSize.size() != ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}

	if(queue.expired())
	{
		throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLKernel::Execute");
	}		
		
	cl_int err = clEnqueueNDRangeKernel(queue.lock().get(), kernel.get(), ndim, nullptr, globalWorkSize.data(), nullptr, 0, nullptr, &event);

	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clEnqueueNDRangeKernel", name.c_str());
	}


	
	return std::move(OpenCLEvent(event));		
}



opencl_helper::OpenCLEvent opencl_helper::OpenCLKernel::Execute(opencl_helper::CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize, const std::vector<size_t> &localWorkSize, opencl_helper::OpenCLEventList &eventWaitList)
{
	cl_event event;
	if(ndim == 0 || 4 <= ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}
	if(globalWorkSize.size() != ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}
	if(localWorkSize.size() != ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}


	if(queue.expired())
	{
		throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLKernel::Execute");
	}
		
	cl_int err = clEnqueueNDRangeKernel(queue.lock().get(), kernel.get(), ndim, nullptr, globalWorkSize.data(), localWorkSize.data(), eventWaitList.size(), eventWaitList.GetList(), &event);

	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clEnqueueNDRangeKernel", name.c_str());
	}


	return std::move(OpenCLEvent(event));	
}

opencl_helper::OpenCLEvent opencl_helper::OpenCLKernel::Execute(opencl_helper::CommandQueueWeakPtr queue, cl_uint ndim, const std::vector<size_t> &globalWorkSize, opencl_helper::OpenCLEventList &eventWaitList)
{
	cl_event event;

	if(ndim == 0 || 4 <= ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}
	if(globalWorkSize.size() != ndim)
	{
		throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
	}

	if(queue.expired())
	{
		throw OpenCLException(CL_INVALID_CONTEXT, "OpenCLKernel::Execute");
	}		
		
	cl_int err = clEnqueueNDRangeKernel(queue.lock().get(), kernel.get(), ndim, nullptr, globalWorkSize.data(), nullptr, eventWaitList.size(), eventWaitList.GetList(), &event);

	if(err != CL_SUCCESS)
	{
		throw OpenCLException(err, "clEnqueueNDRangeKernel", name.c_str());
	}


	
	return std::move(OpenCLEvent(event));		
}



