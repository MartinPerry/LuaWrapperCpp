
printf = function(s,...)
           return io.write(s:format(...))
         end -- function


print("Hello from Lua....")


-- Because the metatable has been exposed 
-- to us, we can actually add new functions
-- to Foo


a = Account(100)
aa = Account(200)

print(a)

r = 12
printf("Old r: %i\n", r)
a:Print1(r)
printf("New r: %i\n", r)

a:Print3("ahoj3", 9, 8)

a:Print0()

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