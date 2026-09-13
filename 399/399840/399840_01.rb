require 'prime'

# a(0)=4, a(1)=0, a(2)=0, a(3)=3; thereafter a(n) = a(n-3) + a(n-4).
def A(n)
  ary = [4, 0, 0, 3]
  (4..n).each{|i|
    ary << ary[i-3] + ary[i-4]
  }
  ary
end
n = 10100
m = 10000
ary = A(n)
# p (1..100).map{|i| ary[i] % i}

(1..m).each{|i|
  j = ary[i] % i
  break if j.to_s.size > 1000
  # a(p) = 0 if p is prime.
  break if i.prime? and j != 0
  print i
  print ' '
  puts j
}
