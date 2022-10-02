require 'prime'

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  h = {}
  Prime.each(n).each{|i|
    h[i] = Math.log(n, i).to_i
  }
  h.group_by{|k, v| v}.inject(1){|s, i| s * f(i.last.size)}
end

def A357328(n)
  [1, 1] + (2..n).map{|i| A(i)}
end

p A357328(100)

# n = 5000
# ary = A357328(n)
# (0..n).each{|i| 
#   j = ary[i]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }
