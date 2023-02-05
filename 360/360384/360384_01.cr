N = 20
D = 7

def check(d, a, i)
  return true if i < d
  !((a[i] - a[i - d]).abs == d)
end

def solve(d, len, a = Array(Int32).new)
  b = Array(Array(Int32)).new
  if a.size == len
    b << a
  else
    (1..len).each{|m|
      s = a.size
      if s == 0 || (s > 0 && !a.includes?(m))
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
