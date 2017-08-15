--test2.lua--

print("test2.lua")

s = {}
for i = 1, 10, 1 do
	s[i] = i
end

print("for k, v in ipairs(s)")
for k, v in ipairs(s) do
	print(k .. "---" .. v)
end

print("for k in pairs(s)") 
for k,v in pairs(s) do
	if k ~= nil
	then
		print(k .. "---" .. v)
	end
end

print("table.unpack(s)")
print(table.unpack(s)) --��� 1 2 3 4 5 6 7 8 9 10

print("table.unpack(s,2)")
print(table.unpack(s,2)) --��� 2 3 4 5 6 7 8 9 10

print("r = table.unpack(s)")
r = table.unpack(s)
print(r) --��� 1

print("r = table.unpack(s,2)")
r = table.unpack(s,2)
print(r) --��� 2

print("select(\"#\",table.unpack(s,2))")
print(select("#",table.unpack(s,2))) --��� 9

print("select(5,table.unpack(s,2))")
print(select(5,table.unpack(s,2))) --��� 6 7 8 9 10

function fun1()--fun1()���ص�����������
    local i = 0
    return function ()      -- �������������ء������Ľ����
        i = i + 1
        return i
    end
end
fun = fun1();
print(fun())     --> 1
print(fun())     --> 2

str1 = "str1: hello lua !"
str2 = "str2: hello world !"
print(table.concat({str1,str2},"\n"))

t = {1,2,3,4,5}
print(getmetatable(t))
t1 = {}
setmetatable(t,t1)
assert(getmetatable(t) == t1)
t1.__index = function(table,key) return t[key] end
print(t[2])

print(_G)

print("package.path = " .. package.path)
print("package.cpath = " .. package.cpath)

require "module"
print(mymodule.new(1))

s = "this 1 test string for lua ."
res = string.find(s,"test")
print(string.find(s,"test"))
res = string.match(s,"test")
print(string.match(s,"test"))
res = string.gsub(s,"test","TEST")
print(string.gsub(s,"test","TEST"))
res = string.gmatch(s,"test")
print(string.gmatch(s,"test")())

print(string.match(s,"(%d+)[%A+](%a+)[%A+](%a+)"))