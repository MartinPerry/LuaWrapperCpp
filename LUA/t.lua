
printf = function(s,...)
           return io.write(s:format(...))
         end -- function

-- https://coronalabs.com/blog/2014/09/02/tutorial-printing-table-contents/
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




print("Hello from Lua....")




-- Because the metatable has been exposed 
-- to us, we can actually add new functions
-- to Foo


a = Account(100)
aa = Account(200)
a2 = Account2(200)
--print_r(getmetatable(a))
--print("-------------")
a:Print0()

--print_r(getmetatable(a))
--print("-------------")
a2:PrintX()

os.exit()



local myTable = {
    firstName = "Fred",
    lastName = "Bob",
    phoneNumber = "(555) 555-1212",
    age = Account(100),
    favoriteSports = { x = "Baseball", "Hockey", "Soccer" },
    favoriteTeams  = { "Cowboys", "Panthers", "Reds" },
	__ = {e = "SUBINDEX"}
}
 
function myTable:add2(x,y)
  return x+y
end

--http://lua-users.org/wiki/MetamethodsTutorial
local mt = {
  __add = function (lhs, rhs) -- "add" event handler
    return { value = lhs.value + rhs.value }
  end,
  __index = function(t, key)
    print("__index called")	
  end,
  __newindex = function(t, key)
    print("__newindex called")
  end,
  xxxx = "Fred"
}

setmetatable(myTable, mt) -- use "mt" as the metatable for "x"
--myTable.lastName = "x"
--myTable.oo = 5
print(myTable.e)
print_r(myTable)
print_r(getmetatable(myTable))

os.exit() 

--print_r(myTable)
print("Index set / get")
--os.exit() 
print("->");
print(a.vv)
--print(a.xx)
--a.cc:PrintX()
local xo = a.cc
a2:PrintX()
print(a.cc)
a.name = 789
a.eco = 789


os.exit() 
print("Account:")
print_r(a)
print("Account metatable:")
print_r(getmetatable(a))

os.exit() 

print(a)

r = 12
printf("Old r: %i\n", r)
a:Print1(r)
printf("New r: %i\n", r)

a:Print3("ahoj3", 9, 8)

printf("-------\n")

a:Print0()
a2:PrintX()

printf("-------\n")

a:Print6(aa)
a:Print6(cc)

a:Print7(aa)
a:Print7(cc)

a = nil
b = nil
collectgarbage()


--c = Account2(100)

Print_fce()
Print_fce2(10)

a:Print0()
a:Print1(10)
a:Print2("ahoj", 9, 8)
a:Print3("ahoj3", 9, 8)
p4 = a:Print4()
p5 = a:Print5(5)

--a:Print6(bb)
--a:Print6(cc)

printf("%i %i\n", p4, p5)


a:printOverload(11)

b:withdraw(25)
printf("B Account balance = $%0.02f\n", b:balance() )
b:Print4(b);
printf("B Account balance = $%0.02f\n", b:balance() )
printf("\n")

a:Print2("ahoj", 9, 8)
a:Print3("ahoj3", 9, 8)
a:Print4(aa);
printf("%f\n", aa:balance())
--a:Print5(aa);
--printf("%f\n", aa:balance())

a:deposit(0)
printf("A Account balance = $%0.02f\n", a:balance() )
a:deposit(50.30)
a:balance2(3)
print("xx")
c:deposit2(50)
printf("C Account balance = $%0.02f\n", c:balance2() )
printf("A Account balance = $%0.02f\n", a:balance() )
a:withdraw(25.10)
printf("A Account balance = $%0.02f\n", a:balance() )


--b:withdraw(25.10)
--printf("B Account balance = $%0.02f\n", b:balance() )



ret = 0

--for i=0,1000000, 1
--do
--ret = ret + LUA_Test(math.random(0,8000))
--end


--ret = ret + LUA_Test(tc, math.random(0,8000))
--ret = ret + LUA_Test(tc, 7)
print(ret)
a = nil
b = nil
collectgarbage()


--ret = LUA_Test2(77);
--print(ret)