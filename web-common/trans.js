let lang
let intUnits
let forceUpdate

const language = window.navigator.userLanguage || window.navigator.language;


// load units from local storage
const unitsPref = localStorage.getItem('intUnits')
intUnits = true
if( unitsPref === '0' )
	intUnits = false
if( unitsPref===null || unitsPref===undefined ) {
	if( language == 'en-US' || language == 'en-us' )
		intUnits = false
}

// load lang from local storage
const langPref = localStorage.getItem('lang')
lang = 'en'
if( langPref ) {
	lang = langPref
}
else {
	lang = language.split('-')[0]
}


export function isIntUnits() {
	return intUnits
}


export function setIntUnits(ok) {
	intUnits = ok
	if( !ok ) {
		localStorage.setItem('intUnits', '0')
	}
	else {
		localStorage.setItem('intUnits', '1')
	}
	forceUpdate()
}


export function currentLang() {
	return lang
}


export function setCurrentLang(l) {
	localStorage.setItem('lang', l)
	lang = l
	forceUpdate()
}


export function transName(names) {
	const l = currentLang()
	if( l in names )
		return names[l]
	if( 'en' in names )
		return names.en
	return ''
}


export function setForceUpdate(func) {
	forceUpdate = func
}


export function transDimItemNameShort(itm) {
	const l = currentLang()
	if( l in itm.nameShort )
		return itm.nameShort[l]
	if( 'en' in itm.nameShort )
		return itm.nameShort.en
	return itm.key
}

export function transDimItemNameLong(itm) {
	const l = currentLang()
	if( l in itm.nameLong )
		return itm.nameLong[l]
	return transDimItemNameShort(itm)
}