def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def solve(d, len, a = [])
  return f(d) if len == 1
  b = []
  if a.size == len
    b << a
  else
    (1..len).each{|m|
      s = a.size
      if s == 0 || (s > 0 && !a.include?(m) && (a[-1] - m).abs >= d)
        b += solve(d, len, a + [m])
      end
    }
  end
  b
end

(2..10).each{|i| p (1..i / 2).map{|j| solve(j, i).size}}
