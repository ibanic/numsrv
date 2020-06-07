function absolutePath(href) {
    const link = document.createElement('a');
    link.href = href;
    return link.href;
}

export default function loadJs(url) {
	return new Promise(function(resolve, reject) {
		const url2 = absolutePath(url)
		const scr = Array.from(document.querySelectorAll('script')).find( (obj)=>{ return obj.src == url2 } )

		if (!scr) {

			// create new script
			let tag = document.createElement('script');
			tag.src = url
			tag.hasLoaded = false
			tag.callbacks = [resolve]
			tag.onload = function() {
				this.hasLoaded = true
				for( const resolve2 of this.callbacks )
					resolve2()
			}
			if( document.getElementsByTagName('body').length > 0 )
				document.getElementsByTagName('body')[0].appendChild(tag);
			else
				document.getElementsByTagName('head')[0].appendChild(tag);
		}
		else {
			if( scr.hasLoaded ) {
				// already loaded
				resolve()
			} else {
				// loading
				if( scr.callbacks !== undefined ) {
					scr.callbacks.push(resolve)
				}
				else {
					resolve()
				}
			}
		}
	})
}