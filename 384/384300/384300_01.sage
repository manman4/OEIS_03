n = 17

# a(n) = RisingFactorial(3*n-2,2*n).
def a(n): return rising_factorial(3*n-2, 2*n)
print([a(n) for n in range(n + 1)])