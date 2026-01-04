# KalaLua

**KalaLua** is a C++ 20 wrapper library around the Lua source code for Windows (Linux coming in the future) that allows you register C++ functions in **KalaLua** that can be called in your Lua script or to call Lua-created functions through **KalaLua** without needing to ever touch Lua source code itself in any way inside your own program unless you want Lua state pointer access.

An example project showcasing how to use **KalaLua** has been created at the [kalakit_test_projects repository](https://github.com/greeenlaser/kalakit_test_projects/tree/main/KalaLua).

## Features

### Easy initialization and shutdown

Just call Lua::Initialize to start and Lua::Shutdown to clean up resources and quit.

### No Lua source code needed

The Lua source code is compiled to the Lua binary but your program does not need to link to it or use any of its source code unless you wish to access the Lua state pointer. Only the KalaLua binary depends on the Lua binary.

### LuaVar-bound variables 

Both CallFunction and the three non-lua RegisterFunction functions accept any number of arguments, or none, as long as theyre within the LuaVar variant range. Passed variables are safely bound-checked against the LuaVar variant.

Allowed variables:
- int
- float
- double
- bool
- string

### Three namespace states

You can call and register functions with no namespace, single parent namespace or nested namespace.

- pass "" to the namespace field to not assign a namespace
- pass "example" to assign a single parent namespace
- pass "example.examplenest" to assign a nested namespace.

### Register functions for use in Lua

Lua::RegisterFunction accepts functionals and free functions and can return void or one of the LuaVar variables.

There is also a third advanced caller which expects you to handle the Lua state pointer, this is only recommended for advanced users as it requires you to link against the Lua source binary and use Lua source code.

### Call Lua functions

You can call a Lua function with Lua::CallFunction which returns nothing or one of the possible LuaVar variables depending on how you've set it up in your Lua script.

---

## Links

[Donate on PayPal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official Discord server](https://discord.gg/jkvasmTND5)

[Official Youtube channel](https://youtube.com/greenlaser)

---

## Docs

[How to build from source](docs/build_from_source.md)

[External libraries](docs/external_libraries.md)

[Lost Empire Entertainment and KalaKit ecosystem](docs/ecosystem.md)
