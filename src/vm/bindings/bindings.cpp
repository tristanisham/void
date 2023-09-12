#include "../vm.hpp"


void vm_serve(WrenVM* vm)
{
    int port
        = wrenGetSlotDouble(vm, 1);  // Assuming the port is passed as a double (Wren doesn't have a native int type)
    int result = vm::serve(port);
    wrenSetSlotNull(vm, 1);
    // wrenSetSlotDouble(vm, 0, result);
}

// http::Response vm::VirtualMachine::handle(const http::Request& request) { }

void vm_router_alloc(WrenVM* vm)
{
    auto router = static_cast<server::Router*>(wrenSetSlotNewForeign(vm, 0, 0, sizeof(server::Router)));
}

void vm_router_finalize(void* data) { delete static_cast<server::Router*>(data); }



void vm_handle_request(WrenVM* vm) {
    
    wrenEnsureSlots(vm, 4); // one extra 🤷‍♂️

    server::Router* router = static_cast<server::Router*>(wrenGetSlotForeign(vm, 0));

    const char* method = wrenGetSlotString(vm, 1);
    const char* path = wrenGetSlotString(vm, 2);
    WrenHandle* ctx = wrenGetSlotHandle(vm, 3);
}