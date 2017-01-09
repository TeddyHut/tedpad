#include "../../../include/tedpad/dataManipulator_file/file.h"

static std::string const _egNAME_FILE_seg_ = __FILE__;

tedpad::DataManipulator::File::File() {
	dataManipulator_loadData_exe = [this](eg::GlbRtrn &rtrn, eg::DataReference &dataReference)->bool { return(this->loadData_impl(rtrn, dataReference)); };
	dataManipulator_freeData_exe = [this](eg::GlbRtrn &rtrn, eg::DataReference &dataReference)->bool { return(this->loadData_impl(rtrn, dataReference)); };
	dataManipulator_writeData_exe = [this](eg::GlbRtrn &rtrn, eg::DataReference const &dataReference)->bool { return(this->writeData_impl(rtrn, dataReference)); };
	
	description[Key::DataReference_Description] = Value::DataReference_Description::File;
}

bool tedpad::DataManipulator::File::loadData_impl(eg::GlbRtrn &rtrn, eg::DataReference &dataReference) {
	static std::string const _egNAME_FUNCTION_seg_ = __FUNCSIG__;
	if (dataReference.description[Key::DataReference_Description] & Value::DataReference_Description::File) {
		std::string const * const path = static_cast<std::string const * const>(dataReference.dataPointer);
		std::ifstream file(*path);
		if (!file) {
			eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
			return(rtrn);
		}
		file.seekg(0, std::ios_base::end);
		dataReference.dataSize = static_cast<size_t>(file.tellg());
		dataReference.dataPointer = static_cast<void *>(new uint8_t[dataReference.dataSize]);
		if (dataReference.dataPointer == nullptr) {
			eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
			return(rtrn);
		}
		file.read(static_cast<char *>(dataReference.dataPointer), dataReference.dataSize);
		if (!file) {
			eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
			return(rtrn);
		}
		file.close();
		eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Success_r);
		return(rtrn);
	}
	eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
	return(rtrn);
}

bool tedpad::DataManipulator::File::freeData_impl(eg::GlbRtrn &rtrn, eg::DataReference &dataReference) {
	static std::string const _egNAME_FUNCTION_seg_ = __FUNCSIG__;
	if (dataReference.description[Key::DataReference_Description] & Value::DataReference_Description::File) {
		delete[] dataReference.dataPointer;
		eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Success_r);
		return(rtrn);
	}
	eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
	return(rtrn);
}

bool tedpad::DataManipulator::File::writeData_impl(eg::GlbRtrn &rtrn, eg::DataReference const &dataReference) {
	static std::string const _egNAME_FUNCTION_seg_ = __FUNCSIG__;
	if (dataReference.description[Key::DataReference_Description] & Value::DataReference_Description::File) {
		std::string const * const path = reinterpret_cast<std::string const * const>(dataReference.alteration);
		std::ofstream file(*path, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
		if (!file) {
			eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
			return(rtrn);
		}
		file.write(static_cast<char *>(dataReference.dataPointer), dataReference.dataSize);
		if (!file) {
			eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
			return(rtrn);
		}
		file.close();
		eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Success_r);
		return(rtrn);
	}
	eg_GlbRtrn_egResult(rtrn, eg::Main_Result_Failure_r);
	return(rtrn);
}
