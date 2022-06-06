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

def search(a, num, n)
  if num == n + 1
    Count.new
  else
    (1..n).each{|i|
      if a[i] == 0
        if i * num / i.gcd(num) <= n
          a[i] = num
          search(a, num + 1, n)
          a[i] = 0
        end
      end
    }
  end
end

def a354756(n)
  a = [0] * (n + 1)
  Count.to_zero
  search(a, 1, n)
  Count.instances
end

(0..20).map{|i| p a354756(i)}
