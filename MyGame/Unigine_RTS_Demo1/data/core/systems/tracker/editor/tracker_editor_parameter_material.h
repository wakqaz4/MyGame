/* Copyright (C) 2005-2015, UNIGINE Corp. All rights reserved.
 *
 * This file is a part of the UNIGINE 2.0-RC SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE Corp. at http://unigine.com/
 */


#ifndef __UNIGINE_TRACKER_EDITOR_PARAMETER_MATERIAL_H__
#define __UNIGINE_TRACKER_EDITOR_PARAMETER_MATERIAL_H__

/*
 */
namespace Unigine::Tracker {
	
	/*
	 */
	class TrackerEditorParameterMaterial : TrackerEditorParameter {
		
		using Unigine::Widgets;
		
		LineCurveMaterial line_curve;
		
		TrackerParameterString::Track parameter_track;
		
		/*
		 */
		TrackerEditorParameterMaterial(TrackerParameterString parameter,vec4 color) : TrackerEditorParameter(parameter) {
			
			parameter_track = new TrackerParameterString::Track();
			
			table_row = new TableRow(getParameterName(),EDITOR_TABLE_NUM_COLUMNS,1,1);
			table_row.setColor(color);
			table_row.setData(this);
			
			line_curve = new LineCurveMaterial(color);
			line_curve.setData(this);
		}
		
		/*
		 */
		void addParameter(Table table,Track track,Line line,TrackerTrack tracker_track) {
			
			add_parameter(tracker_track,parameter_track);
			
			table.addRow(table_row);
			line.addCurve(line_curve);
		}
		
		/*
		 */
		void setTrack(int track) {
			delete parameter_track;
			parameter_track = parameter.cloneTrack(track);
		}
		
		int getTrack() {
			return parameter_track;
		}
		
		/*
		 */
		void updateTrack() {
			parameter_track.keys.delete();
			foreach(LineKeyMaterial key; line_curve.keys) {
				TrackerParameter::KeyString track_key = new TrackerParameter::KeyString();
				track_key.time = key.getTime();
				track_key.value = key.getMaterial();
				parameter_track.keys.append(track_key);
			}
			parameter.updateTrack(parameter_track);
		}
		
		/*
		 */
		void updateCurve() {
			line_curve.clear();
			foreach(TrackerParameter::KeyString key; parameter_track.keys) {
				LineKeyMaterial line_key = new LineKeyMaterial(key.time,(strlen(key.value) > 0),key.value);
				line_curve.addKey(line_key);
			}
		}
	};
	
} /* namespace Unigine::Tracker */

#endif /* __UNIGINE_TRACKER_EDITOR_PARAMETER_MATERIAL_H__ */
