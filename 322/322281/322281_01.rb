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

def A322281(n)
  (0..n).map{|i| solve(4, i).size}
end

p solve(4, 17).map{|i| [(0..15).map{|j| i[j] - i[j + 1]}.select{|k| k.abs < 4}, (0..14).map{|j| i[j] - i[j + 2]}.select{|k| k.abs < 4}, (0..13).map{|j| i[j] - i[j + 3]}.select{|k| k.abs < 4}]}
p A322281(18)
p solve(4, 19).size
p solve(4, 20).size
