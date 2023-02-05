N = 20
D = 8

def check(d, a, i)
  return true if i < d
  !((a[i] - a[i - d]).abs == d)
end

def solve(d, len, a = [])
  b = []
  if a.size == len
    b << a
  else
    (1..len).each{|m|
      s = a.size
      if s == 0 || (s > 0 && !a.include?(m))
        if check(d, a + [m], s)
          b += solve(d, len, a + [m])
        end
      end
    }
  end
  b
end

def a(d, b)
  return 1 if b == 0
  solve(d, b).size
end

(0..N).each{|i| p a(D, i)}

# [1, 1, 2, 6, 24, 120, 720, 4800, 34752, 280512, 2528256, 25282560]
# [1, 1, 2, 6, 24, 120, 720, 5040, 38880, 323520, 2953728, 29666304]
# [1, 1, 2, 6, 24, 120, 720, 5040, 40320, 352800, 3312000, 33742080]

