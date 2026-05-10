# Elypso scripting language

## Introduction

ESL (Elypso scripting language) is a DSL (Domain specific language) which is used as a full replacement for C++ if the user wishes to not write any C++ code. Entire games and software can be made without ever touching any C++ code in Elypso Engine by just using ESL. ESL uses the .esl extension for scripts and .esb (Elypso Script Binary) for compiled games and software.

ESL does not use semicolons or brackets, instead scope qualifiers like if and endif, or start and endstart are used.

Every script must contain a start, endstart, update and endupdate. A script which does not have its start or update qualifiers filled cannot be ran.

Functions are called actions, their syntax is different from regular C++ functions
```
action myaction(string arg1, string arg2) : string
    return (arg1 + ", " + arg2)
endaction
```

There are three major categories:
- operators
- variables
- flags
- scope qualifiers

Operators split into three categories:
- assignment operators
- comparison operators
- math operators

Variables split into three categories:
- data holders
- collection types

## Operators

### Assignment operators

- =
- as
```
//A special assignment operator meant to assign a default value to an entity.

inspectorpanel pa

inputfield in
string st

slider sl
f32 fl

slider sl2
f32 fl2

start
    //the input field name is set to the value of st
    pa.attachentity(in "inputfield" as value st)
    //the slider min value is set to the value of fl
    pa.attachentity(sl "myslider" as minvalue fl)
    //the slider value is set to the value of fl2
    pa.attachentity(sl2 "myslider2" as value fl2)
endstart
```

### Comparison operators

- ==
- !=
- <=
- >=

### Math operators

- +
- -
- +=
- -=
- *
- /
- *=
- /=

## Variables

Use the `ref` keyword in front of a variable to mark it as a reference, not a full new variable of its kind.
```
script a
    triggersphere main
endscript

script b
    ref triggersphere tsp = a.main
endscript
```

### Data holders

- string
- bool
- u8, u16, u32, u64
```
u8 - 0-255,
u16 - 0-65535
u32 - 0-4,294,967,295
u64 - 0-18 quintillion
```
- i8, i16, i32, i64
```
i8 - -128-127,
i16 - -32,768-32,767
i32 - -2,147,483,648-2,147,483,647
i64 - -9 quintillion-9 quintillion
```
- f32, f64
```
f32 - float,
f64 - double
```

- container
```
A container holds multiple variable types of any kind, it acts like a struct but cannot contain functions
```

### Collection types

- list
```
A list is like a vector, it holds data of one type
```

## Flags

Flags define stuff when the script is attached, same idea as defines, but they cannot be used in start or update and cannot be edited from the same or another script once set

External scripts not in the same esl file can be referenced with the add flag
```
script myscript
    add myotherscript "../myotherscript.esl"
    
    i8 myval
    
    start
        myval = myotherscript.myval
    endstart
endscript

//inside myotherscript.esl
script scr
    i8 myval = -250
endscript
```

Example custom flags
```
script myscript
    priority 100
    ignoredeath true
endscript
```

## Scope qualifiers

Scope qualifiers mark the start and end of a code block.

The contents inside start are ran once when the script is attached. Update runs forever until `this.deattach(myscriptname)` or `this.pause(myscriptname)` is called, to continue a paused script simply call `this.continue(myscriptname)`.

- script, endscript
- start, endstart
- update, endupdate
- action, endaction
- if, elseif, else, endif
- for, endfor
- while, endwhile
