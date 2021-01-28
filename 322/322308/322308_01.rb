def check(d, a, i)
  return true if i == 0
  j = 1
  d_max = [i, d - 1].min
  while (a[i] - a[i - j]).abs >= d && j < d_max
    j += 1
  end
  (a[i] - a[i - j]).abs >= d
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

def A322308(n)
  (0..n).map{|i| solve(5, i).size}
end

p A322308(25)
# p solve(5, 26).size
# p solve(5, 27).size
# p solve(5, 28).size
