def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/3)} (3*k+1) * binomial(n-1,3*k) * a(3*k) * a(n-1-3*k).
def a(n, x)
  xs = x.size
  ary = [1]
  (1..n).each{|m|
    s = 0
    (0..m - 1).to_a.repeated_permutation(xs).each{|i|
      # x[k] * i[k]
      y = i.map.with_index{|v, k| x[k] * v}
      s += (m - y[0]) * (1..xs).inject(1r){|prod , j| prod * ary[y[j- 1]] / f(y[j - 1])} if y.sum == m - 1
    }
    ary << (f(m - 1) * s).to_i
  }
  ary
end

n = 23
p a(n, [1, 3]) 