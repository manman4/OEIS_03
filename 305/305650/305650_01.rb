p (1..100).map{|n| (- ((1 + Math.sqrt(2)) ** n - 1) * ((1 - Math.sqrt(2)) ** n - 1)).round}
p (1..100).map{|n| (- ((1 + Math.sqrt(3)) ** n - 1) * ((1 - Math.sqrt(3)) ** n - 1) / 3).round}

def A(n)
  a, b, c, d = 1, 1, 9, 13
  ary = [1]
  while ary.size < n
    a, b, c, d = b, c, d, d + 6 * c - 2 * b - 4 * a
    ary << a
  end
  ary[0..n - 1]
end

n = 100
p ary = A(n)
