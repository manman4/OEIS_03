require 'prime'

# Number of nonnegative integer solutions to the equation x^2 + 4*y^2 = n.
def A(n)
  m = Math.sqrt(n).to_i + 1
  ary = Array.new(n + 1, 0)
  (0..m).each{|i|
    (0..m).each{|j|
      k = i * i + 4 * j * j
      if k <= n
        ary[k] += 1
      end
    }
  }
  ary
end

n = 10000
ary = A(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
 
