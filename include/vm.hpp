#pragma once
#include "wren.hpp"
#include <exception>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <variant>
#include <vector>

namespace vm {

size_t createVmMap(WrenVM* vm, const size_t& slot, const std::unordered_map<std::string, std::string>& entries);



std::optional<std::string> getOperatingSystem();
std::optional<std::string> getPlatformArchitecture();
class Runtime {
public:
	Runtime();
	WrenInterpretResult execute(const std::string& code, const std::string& module = "main");
	void repl();
	void setEntryPoint(const std::filesystem::path& target);
	static void setProgramArgs(int argc, char** argv);
private:
	std::shared_ptr<WrenVM> vm;
	std::filesystem::path entryPoint;
};

}  // namespace vm
