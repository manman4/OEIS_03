# a(n): Number of set partitions of [n] = {1, 2, ..., n} into nonempty subsets
#       such that the element sums of the blocks are DISTINCT primes.
#
# Notes:
#   - Blocks are unordered and the number of blocks is unrestricted.
#   - Each block sum must be prime (so 0 and 1 are never valid block sums;
#     in particular the singleton {1} can never be its own block).
#   - "Distinct" means no two blocks may share the same prime sum.
#
# Method: anchor the recursion on the smallest not-yet-placed element, so every
# set partition is generated exactly once. A running set of already-used prime
# sums enforces distinctness. (No plain memoization on the element mask alone:
# the distinctness constraint couples the branches, so the used-primes set is
# part of the state.)

require 'prime'
require 'set'

def prime_sum?(x)
  x >= 2 && Prime.prime?(x)
end

def distinct_prime_set_partitions(n)
  full = (1 << n) - 1

  # Precompute the element-sum of every subset (bit i-1 <-> element i).
  block_sum = Array.new(1 << n, 0)
  (0...(1 << n)).each do |mask|
    s = 0
    n.times { |i| s += i + 1 if mask[i] == 1 }
    block_sum[mask] = s
  end

  # rec.(mask, used): number of valid partitions of the elements in `mask`,
  # given that the prime sums in `used` are already taken.
  rec = lambda do |mask, used|
    return 1 if mask.zero?
    low  = mask & -mask          # bit of the smallest remaining element
    rest = mask ^ low            # elements that may join its block
    total = 0
    sub = rest
    loop do                      # iterate over all subsets of `rest`
      block = low | sub          # block must contain the smallest element
      s = block_sum[block]
      if prime_sum?(s) && !used.include?(s)
        used.add(s)
        total += rec.call(mask ^ block, used)
        used.delete(s)
      end
      break if sub.zero?
      sub = (sub - 1) & rest
    end
    total
  end

  rec.call(full, Set.new)
end

if __FILE__ == $PROGRAM_NAME
  upto = (ARGV[0] || 14).to_i
  p [1] + (1..upto).map{|n| distinct_prime_set_partitions(n)}
end
