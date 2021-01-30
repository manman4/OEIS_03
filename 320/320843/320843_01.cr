class Count
  @@instances = 0
  def initialize
    @@instances += 1
  end
  def self.instances
    @@instances
  end
  def self.to_zero
    @@instances = 0
  end
end

def search(a, num, n)
  if num == n + 1
    Count.new
  else
    (1..n).each{|i|
      if a[i] == 0
        if num % i == 0 || i % num == 0
          a[i] = num
          search(a, num + 1, n)
          a[i] = 0
        end
      end
    }
  end
end

def a320843(n)
  a = [0] * (n + 1)
  Count.to_zero
  search(a, 1, n)
  Count.instances
end

p (0..20).map{|i| a320843(i)}