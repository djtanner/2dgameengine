some_variable = 54

if some_variable == 42 then
    print("The answer to the ultimate question of life, the universe and everything is " .. some_variable)
else
    print("The answer to the ultimate question of life, the universe and everything is not " .. some_variable)
end

function factorial(n)
    if n == 0 then
        return 1
    else
        return n * factorial(n - 1)
    end
end

print("The factorial of 5 is " .. factorial(5))