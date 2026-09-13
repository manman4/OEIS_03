require 'prime'

# a(n) = a(n-4) + a(n-5), with a(0)=5, a(1)=a(2)=a(3)=0.
def A(n)
  ary = [5, 0, 0, 0, 4]
  (5..n).each{|i|
    ary << ary[i-4] + ary[i-5]
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
