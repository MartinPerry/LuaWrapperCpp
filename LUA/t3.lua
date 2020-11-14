function print_r ( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        print(indent.."["..pos.."] => "..tostring(t).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        print(indent.."["..pos..'] => "'..val..'"')
                    else
                        print(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else				
                print(indent.."Object: "..tostring(t))				
								
            end
        end
    end
    if (type(t)=="table") then
        print(tostring(t).." {")
        sub_print_r(t,"  ")
        print("}")
    else
        sub_print_r(t,"  ")
    end
    print()
end

--[=====[ 
local mt = 
{
	__index = function (t, k)  -- {} an empty table, and after the comma, a custom function failsafe
		print(t)
		return "key doesn't exist"
	end
}


local A = setmetatable({ a = 10}, mt)
local B = setmetatable({ b = 10}, mt)

print_r(A)
print_r(getmetatable(A))


print(A.a)
print(A.c)
print(B.c)

os.exit()
--]=====]

local func_example = setmetatable(
{
sy = "y"
}, 
{
	__index = function (t, k)  -- {} an empty table, and after the comma, a custom function failsafe
		 print(debug.traceback())
		return "key doesn't exist"
	end
}
)

local fallback_tbl = setmetatable(
{   
  foo = "bar",
  [123] = 456
},  -- table
{
	__index = func_example
}
)

local fallback_example = setmetatable(
{	
},  --table
{	
	__index=fallback_tbl
}
)

setmetatable(func_example, fallback_example)

function fallback_example:add2(x,y)
  return x+y
end


t = TestClass(150)
t:Print0()
t:Print0_args(1, 2)
local r = t:Print2()
t:Print3(t)
t:Print4(t)
t:Print5()
HelloWorld("x", tt)

--print_r(fallback_example)
--print_r(getmetatable(fallback_example))

v = Vector2();
v.x = 15

print(v)

a = Account_2(150, 0)

print(a.v2)
a.v2 = v
print(a.v2)

print(a.vv)
print(a)
--os.exit()

a:PrintConst()

--os.exit()
--a:Print7(ee)

--a:Print7(cc)
--a:Print7(ee)
--ee:Print0()
a:Print0();
a:Print0L(70, 0);
print("xxxx")
print(a)
print(getmetatable(a))
print("xxxx")
--print_r(ee)
--print_r(getmetatable(ee))
--print_r(getmetatable(getmetatable(ee)))
print("_____")
print_r(a)
print_r(getmetatable(a))
print("_____")
--os.exit()

a:Print1(123);

print("148 / " .. a.vv)
a.vv = 44
print("44 / " .. a.vv)


--print(fallback_example.sy)
--print(fallback_example.k)
--print(fallback_example:add2(1,2))
--print(fallback_example.foo) --> bar
--print(fallback_example[123]) --> 456
--print(fallback_example[456]) --> key doesn't exist

--os.exit()

function testVoid(x, y)
	print("testVoid: " .. x .. " / " .. y)
end	

function add(x, y)
	return x + y 
end	