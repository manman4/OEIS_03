# 符号は無視
def stirling(n, k = 1)
  a = [1]
  (1..n).each{|i|
    a << 0
    b = [0]
    (0..i - 1).each{|j|
      if k == 2
        b[j + 1] = a[j] + (j + 1) * a[j + 1]
      else
        b[j + 1] = a[j] + (i - 1) * a[j + 1]
      end
    }
    a = b
  }
  a
end

# a(n) = Sum_{0 <= x_1, x_2,..., x_n <= n and x_1 + x_2 + ... + x_n = (n-1)*n} Product_{k=1..n} |Stirling1(n,x_k)|.
def A(n)
  return 1 if n == 0
  s1abs_n = stirling(n)
  sum = 0
  (0..n).to_a.repeated_permutation(n){|perm|
    if perm.inject(:+) == (n - 1) * n
      sum += perm.inject(1){|s, i| s * s1abs_n[i]}
    end
  }
  sum
end

n = 8
p (0..n).map{|i| A(i)}

