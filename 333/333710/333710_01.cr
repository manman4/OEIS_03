require "big"

class Count
  @@cnt = 0
  def initialize
    @@cnt += 1
  end
  def self.instances
    @@cnt
  end
  def self.to_zero
    @@cnt = 0
  end
end

def f(n)
  return 1 if n < 2
  s = 1
  (1..n).each{|i| s *= i}
  s
end

def search(a, num, n, prod)
  if num == n + 1
    #p a
    Count.new
  else
    (1..n).each{|i|
      if a[i] == 0
        prod *= i
        if prod % f(num) == 0
          
          a[i] = num
          #p [prod, a]
          search(a, num + 1, n, prod)
          a[i] = 0
        end
        prod /= i
      end
    }
  end
end
def a(n)
  a = [0] * (n + 1)
  Count.to_zero
  search(a, 1, n, 1)
  Count.instances
end



p (1..21).map{|i| p a(i)}

