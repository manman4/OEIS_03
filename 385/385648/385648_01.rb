# a(0) = 1; a(n) = Sum_{i, j, k>=0 and i+j+k=n-1} (2^j+1) * (2^k+1) * a(i) * a(j) * a(k).
def a(n, k)
  ary = [1]
  (1..n).each{|m|
    s = 0
    # i,j,k>=0 and i+j+k=m-1を列挙
    (0..m - 1).to_a.repeated_permutation(k).each{|i|
      s += ary[i[0]] * (2..k).inject(1){|prod, j| prod * (2 ** i[j - 1] + 1) * ary[i[j - 1]]} if i.sum == m - 1
    }
    ary << s
  }
  ary
end

p a(30, 3)
