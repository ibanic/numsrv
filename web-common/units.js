// int unit, us unit, int*factor=us
const cnv = [
	['km', 'miles', 0.62137119224],
]

export function convertUnits(n, unit) {
	for( const pair of cnv ) {
		if( pair[0] == unit ) {
			return n * pair[2]
		}
	}
	return n
}

export function usUnitName(unit) {
	for( const pair of cnv ) {
		if( pair[0] == unit ) {
			return pair[1]
		}
	}
	return unit
}