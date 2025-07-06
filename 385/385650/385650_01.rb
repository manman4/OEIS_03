def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(0) = 1; a(n) = (n-1)! * Sum_{i, j, k>=0 and i+j+k=n-1} (n-i) * (2^j+1) * (2^k+1) * a(i) * a(j) * a(k)/(i! * j! * k!).
def a(n, k)
  ary = [1]
  (1..n).each{|m|
    s = 0
    # i,j,k>=0 and i+j+k=m-1を列挙
    (0..m - 1).to_a.repeated_permutation(k).each{|i|
      s += (m - i[0]) * ary[i[0]] / f(i[0]).to_r * (2..k).inject(1r){|prod, j| prod * (2 ** i[j - 1] + 1) * ary[i[j - 1]] / f(i[j - 1])} if i.sum == m - 1
    }
    ary << (f(m - 1) * s).to_i
  }
  ary
end

p a(30, 3)