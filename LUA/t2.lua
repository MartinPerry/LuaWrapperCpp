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




local func_example = setmetatable(
{
sy = "y"
}, 
{
	__index = function (t, k)  -- {} an empty table, and after the comma, a custom function failsafe
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

--print_r(fallback_example)
--print_r(getmetatable(fallback_example))


a = Account(150)
a:Print7(cc)
a:Print7(ee)
ee:Print0()
print("xxxx")
print(a)
print(getmetatable(a))
print(getmetatable(getmetatable(a)))
print("xxxx")
--print("Table")
print_r(a)
--print("Metatable")
print_r(getmetatable(a))
print_r(getmetatable(getmetatable(a)))
print("_____")


a:Print1(123);
print(a.vv)
print(a.cc)
--print(a.cc:PrintX())


--os.exit()

--print(fallback_example.sy)
--print(fallback_example.k)
--print(fallback_example:add2(1,2))
--print(fallback_example.foo) --> bar
--print(fallback_example[123]) --> 456
--print(fallback_example[456]) --> key doesn't exist

--os.exit()