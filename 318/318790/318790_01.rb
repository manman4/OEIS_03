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
def A318790(n)
  (1..n).map{|i| solve(i, i * i + 1).size / 2}
end
p A318790(2)
p solve(2, 5)
