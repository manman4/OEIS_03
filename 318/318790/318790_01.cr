def check(d, a, i)
  return true if i == 0
  j = 1
  d_max = [i, d - 1].min
  while (a[i] - a[i - j]).abs >= d && j < d_max
    j += 1
  end
  (a[i] - a[i - j]).abs >= d
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

def a318790(n)
  (1..n).map{|i| solve(i, i * i + 1).size // 2}
end

p a318790(4)
