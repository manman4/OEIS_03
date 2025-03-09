def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(n) = Sum_{j=1..floor(n/2)}(n!/((2^j)j!(n-2j)!))*a(n-j), a(1)=a(2)=1.
def A(n)
  ary= [0, 1, 1]
  (3..n).each{|i|
    ary << (1..i / 2).inject(0){|s, j| s + f(i) / (2r ** j * f(j) * f(i - 2 * j)) * ary[i - j]}.to_i
  }
  ary
end

n = 300
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
