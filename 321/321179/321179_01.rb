def s(k, n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0 && i % k == 0}
  s
end

def A(ary, n)
  a_ary = [1]
  a = [0] + (1..n).map{|i| ary.inject(0){|s, j| s += j[1] * s(j[0], i)}}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s -= a[j] * a_ary[-j]} / i}
  a_ary
end

def B(k, n)
  ary = []
  (1..k).each{|i|
    ary << [2 * i, 5]
    ary << [i, -2]
    ary << [4 * i, -2]
  }
  A(ary, n)
end

def C(n)
  a = B(n, n * n)[-1]
end

n = 20
ary = (0..n).map{|i| C(i)}
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

# (0..3).to_a.repeated_permutation(3){|a,b,c|
#   p [a,b,c] if a*a+2*b*b+3*c*c==9
# }
