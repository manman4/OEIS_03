def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(k, n)
  sum = 0
  m = f((k - 1) * n)
  (0..n).to_a.repeated_permutation(k){|i|
    if (0..k - 1).inject(0){|s, j| s + i[j]} == n
      sum += m / (0..k - 1).inject(1){|s, j| s * f(n - i[j])}
    end
  }
  sum
end

def A348703(n)
  (0..n).map{|i| A(i, i)}
end

p A348703(8)

