require 'prime'

def A(n)
  return 1 if n == 0
  a = [0, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 8, 8, 8, 8, 9, 9, 9]
  b = a.size - 1
  (2..10 ** 10).each{|num|
    a.shift
    m = a[-1]
    m += 1 if (num + b).prime?
    a << m
    if a[Math.log(num).to_i] - a[0] == n
      return num
      break
    end
  }
end

p (0..5).map{|i| A(i)}