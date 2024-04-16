#ifndef TOOL_H
# define TOOL_H

# define SET(PORT, N) (PORT |= (1 << N))
# define RESET(PORT, N) (PORT &= ~(1 << N))
# define TOGGLE(PORT, N) (PORT ^= (1 << N))

#endif