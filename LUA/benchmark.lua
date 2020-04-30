

a = Account(150)

for i=1, 100 do
	a:deposit(i)
end

print(a:balance())

for i=1, 100 do
	a.vv = a.vv + i
end

print(a.vv)